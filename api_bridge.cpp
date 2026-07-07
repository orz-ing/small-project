#include "backend.h"
#include "utils.h"
#include <sqlite3.h>
#include <memory>

// ===== Singleton =====
ApiBridge& ApiBridge::getInstance() {
    static ApiBridge instance;
    return instance;
}

ApiBridge::~ApiBridge() {
    if (db_) { db_->close(); delete db_; db_ = nullptr; };
}

// ===== Initialize =====
bool ApiBridge::initialize(const string& dbPath) {
    db_ = new DatabaseManager();
    Status s = db_->open(dbPath);
    if (!s.ok()) return false;

    userDao_ = new UserDao(*db_);
    bookDao_ = new BookDao(*db_);
    brDao_ = new BorrowRecordDao(*db_);
    catDao_ = new CategoryDao(*db_);
    resDao_ = new ReservationDao(*db_);
    logDao_ = new LogDao(*db_);

    searchIdx_ = new SearchIndex();
    recommendEng_ = new RecommendEngine();
    categoryTree_ = new CategoryTree();
    creditEng_ = new CreditEngine();
    fineCalc_ = new FineCalculator();
    resQueue_ = new ReservationQueue();

    logSvc_ = make_unique<LogService>(logDao_);
    authSvc_ = make_unique<AuthService>(userDao_);
    bookSvc_ = make_unique<BookService>(bookDao_, logSvc_.get(), searchIdx_, recommendEng_, categoryTree_);
    borrowSvc_ = make_unique<BorrowService>(brDao_, bookDao_, creditEng_, resQueue_);
    resSvc_ = make_unique<ReservationService>(resDao_, resQueue_);
    scanner_ = new OverdueScanner(brDao_, bookDao_, creditEng_);
    overdueScanner_ = unique_ptr<OverdueScanner>(scanner_);

    return true;
}

// ===== Authentication =====
User ApiBridge::login(const string& username, const string& password, Status& status) {
    User user = authSvc_->login(username, password, status);
    if (status.ok()) currentUser_ = user;
    return user;
}

User ApiBridge::registerUser(const string& username, const string& password,
    const string& realName, const string& role, Status& status) {
    return authSvc_->registerUser(username, password, realName, role, status);
}

User ApiBridge::getCurrentUser() const { return currentUser_; }

// ===== Search =====
pair<vector<Book>, int> ApiBridge::searchBooks(const string& query, int categoryId,
    int page, int pageSize, Status& status, bool& done) {
    return bookSvc_->search(query, categoryId, page, pageSize, status, done);
}

Book ApiBridge::getBookDetail(int bookId) {
    return bookSvc_->getDetail(bookId);
}

vector<Book> ApiBridge::getRecommendations(int userId, Status& status, bool& done) {
    return bookSvc_->getRecommendations(userId, 10, status, done);
}

// ===== Borrow =====
BorrowRecord ApiBridge::borrowBook(int userId, int bookId, Status& status, bool& done) {
    done = false;
    User user = userDao_->findById(userId);
    if (user.getId() == 0) { done = true; status = {StatusCode::ERR_USER_NOT_FOUND, "user not found"}; return BorrowRecord(); }
    if (!user.isActive()) { done = true; status = {StatusCode::ERR_USER_DISABLED, "user disabled"}; return BorrowRecord(); }
    if (!user.canBorrow()) { done = true; status = {StatusCode::ERR_CREDIT_TOO_LOW, "credit too low"}; return BorrowRecord(); }
    return borrowSvc_->borrowBook(userId, bookId, status, done);
}

BorrowRecord ApiBridge::returnBook(int borrowId, Status& status, bool& done) {
    done = false;
    auto* db = (sqlite3*)db_->getHandle();
    const char* sql = "SELECT * FROM borrow_records WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        done = true; status = {StatusCode::ERR_DB_EXEC, "query failed"}; return BorrowRecord();
    }
    sqlite3_bind_int(stmt, 1, borrowId);
    BorrowRecord record;
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        record.setId(sqlite3_column_int(stmt,0));
        record.setUserId(sqlite3_column_int(stmt,1));
        record.setBookId(sqlite3_column_int(stmt,2));
        record.setBorrowDate((const char*)sqlite3_column_text(stmt,3));
        record.setDueDate((const char*)sqlite3_column_text(stmt,4));
        record.setReturnDate(sqlite3_column_text(stmt,5)?(const char*)sqlite3_column_text(stmt,5):"");
        record.setStatus((const char*)sqlite3_column_text(stmt,6));
        record.setRenewCount(sqlite3_column_int(stmt,7));
        found = true;
    }
    sqlite3_finalize(stmt);
    if (!found) { done = true; status = {StatusCode::ERR_BORROW_NOT_FOUND, "record not found"}; return BorrowRecord(); }
    if (record.getStatus() == "returned") { done = true; status = {StatusCode::ERR_UNKNOWN, "already returned"}; return BorrowRecord(); }

    string returnDate = today();
    int overdueDays = daysBetween(record.getDueDate(), returnDate);
    double fine = FineCalculator::calcFine(record.getDueDate(), returnDate);

    Status s1 = brDao_->updateReturn(borrowId, returnDate);
    if (!s1.ok()) { done = true; status = s1; return BorrowRecord(); }

    int userId = record.getUserId();
    User user = userDao_->findById(userId);
    if (overdueDays > 0) {
        int newCredit = CreditEngine::updateScore(user.getCredit(), "overdue");
        userDao_->updateCredit(userId, newCredit);
    } else {
        int newCredit = CreditEngine::updateScore(user.getCredit(), "on_time");
        userDao_->updateCredit(userId, newCredit);
    }

    auto pending = resDao_->findByBook(record.getBookId());
    if (!pending.empty()) {
        int nextUserId = pending[0].getUserId();
        logSvc_->log(0, "reservation_fulfilled", "book:" + to_string(record.getBookId()),
            "user:" + to_string(nextUserId));
    }

    record.setReturnDate(returnDate);
    record.setStatus(overdueDays > 0 ? "overdue" : "returned");
    done = true; status = StatusOK();
    return record;
}

BorrowRecord ApiBridge::renewBook(int borrowId, Status& status, bool& done) {
    done = false;
    auto* db = (sqlite3*)db_->getHandle();
    const char* sql = "SELECT * FROM borrow_records WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        done = true; status = {StatusCode::ERR_DB_EXEC, "query failed"}; return BorrowRecord();
    }
    sqlite3_bind_int(stmt, 1, borrowId);
    BorrowRecord record; bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        record.setId(sqlite3_column_int(stmt,0)); record.setUserId(sqlite3_column_int(stmt,1));
        record.setBookId(sqlite3_column_int(stmt,2)); record.setBorrowDate((const char*)sqlite3_column_text(stmt,3));
        record.setDueDate((const char*)sqlite3_column_text(stmt,4));
        record.setReturnDate(sqlite3_column_text(stmt,5)?(const char*)sqlite3_column_text(stmt,5):"");
        record.setStatus((const char*)sqlite3_column_text(stmt,6));
        record.setRenewCount(sqlite3_column_int(stmt,7)); found = true;
    }
    sqlite3_finalize(stmt);
    if (!found) { done = true; status = {StatusCode::ERR_BORROW_NOT_FOUND, "not found"}; return BorrowRecord(); }
    if (record.getStatus() != "borrowing") { done = true; status = {StatusCode::ERR_UNKNOWN, "not borrowing"}; return BorrowRecord(); }
    if (record.getRenewCount() >= 2) { done = true; status = {StatusCode::ERR_RENEW_LIMIT, "max renew reached"}; return BorrowRecord(); }

    string newDueDate = addDays(record.getDueDate(), 15);
    Status s = brDao_->updateRenew(borrowId, newDueDate);
    if (!s.ok()) { done = true; status = s; return BorrowRecord(); }
    record.setDueDate(newDueDate); record.setRenewCount(record.getRenewCount() + 1);
    done = true; status = StatusOK(); return record;
}

vector<BorrowRecord> ApiBridge::getMyBorrowRecords(int userId) {
    return brDao_->findByUser(userId);
}

// ===== Reservation =====
Reservation ApiBridge::reserveBook(int userId, int bookId, Status& status) {
    return resSvc_->reserveBook(userId, bookId, status);
}

Status ApiBridge::cancelReservation(int reservationId) {
    return resSvc_->cancelReservation(reservationId);
}

vector<Reservation> ApiBridge::getMyReservations(int userId) {
    return resSvc_->getMyReservations(userId);
}

// ===== Admin =====
Status ApiBridge::addBook(const Book& book) {
    return bookSvc_->addBook(book, currentUser_.getId());
}

Status ApiBridge::updateBook(const Book& book) {
    return bookSvc_->updateBook(book, currentUser_.getId());
}

Status ApiBridge::deleteBook(int bookId, int adminId) {
    return bookSvc_->deleteBook(bookId, adminId);
}

vector<User> ApiBridge::getAllUsers() {
    return userDao_->findAll();
}

Status ApiBridge::disableUser(int userId, int adminId) {
    Status s = userDao_->updateStatus(userId, "disabled");
    if (s.ok()) logSvc_->log(adminId, "disable_user", to_string(userId), "");
    return s;
}

Status ApiBridge::enableUser(int userId) {
    Status s = userDao_->updateStatus(userId, "active");
    if (s.ok()) logSvc_->log(currentUser_.getId(), "enable_user", to_string(userId), "");
    return s;
}

// ===== Statistics =====
ApiBridge::Stats ApiBridge::getStatistics(Status& status, bool& done) {
    done = false;
    Stats st = {};
    auto allBooks = bookDao_->findAll();
    st.totalBooks = (int)allBooks.size();
    auto allUsers = userDao_->findAll();
    st.totalUsers = (int)allUsers.size();

    auto* db = (sqlite3*)db_->getHandle();
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM borrow_records WHERE status='borrowing'", -1, &stmt, nullptr);
    if (sqlite3_step(stmt) == SQLITE_ROW) st.activeBorrows = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt); stmt = nullptr;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM borrow_records WHERE status='overdue' OR (status='borrowing' AND due_date<datetime('now','localtime'))", -1, &stmt, nullptr);
    if (sqlite3_step(stmt) == SQLITE_ROW) st.overdueCount = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    done = true; status = StatusOK();
    return st;
}
