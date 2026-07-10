#include "bridge/api_bridge.h"
#include <QDebug>
#include <QSqlQuery>

ApiBridge* ApiBridge::instance() {
    static ApiBridge inst;
    return &inst;
}

ApiBridge::ApiBridge() : QObject(nullptr) {}

ApiBridge::~ApiBridge() {
    shutdown();
}

bool ApiBridge::initialize(const QString& dbPath) {
    if (m_initialized) return true;

    // 鍒濆鍖栨暟鎹簱
    if (!DatabaseManager::instance().initialize(dbPath)) {
        qCritical() << "Database initialization failed";
        return false;
    }

    QSqlDatabase db = DatabaseManager::instance().database();

    // 鍒涘缓鍚勬湇鍔?
    m_authService = new AuthService(db);
    m_bookService = new BookService(db);
    m_borrowService = new BorrowService(db);
    m_reservationService = new ReservationService(db);
    m_overdueScanner = new OverdueScanner(db);
    m_logService = new LogService(db);
    m_searchIndex = new SearchIndex();
    m_recommendEngine = new RecommendEngine();
    m_categoryTree = new CategoryTree();
    m_creditEngine = new CreditEngine();

    // 杩炴帴閫炬湡鎵弿淇″彿
    connect(m_overdueScanner, &OverdueScanner::overdueFound,
            this, &ApiBridge::overdueFound);

    // 鍔犺浇宸叉湁鏁版嵁鍒扮储寮?
    rebuildIndex();

    // 鍚姩閫炬湡鎵弿
    m_overdueScanner->start();

    m_initialized = true;
    qInfo() << "ApiBridge initialized successfully";
    return true;
}

void ApiBridge::shutdown() {
    if (!m_initialized) return;

    delete m_overdueScanner;  m_overdueScanner = nullptr;
    delete m_authService;     m_authService = nullptr;
    delete m_bookService;     m_bookService = nullptr;
    delete m_borrowService;   m_borrowService = nullptr;
    delete m_reservationService; m_reservationService = nullptr;
    delete m_logService;      m_logService = nullptr;
    delete m_searchIndex;     m_searchIndex = nullptr;
    delete m_recommendEngine; m_recommendEngine = nullptr;
    delete m_categoryTree;    m_categoryTree = nullptr;
    delete m_creditEngine;    m_creditEngine = nullptr;

    DatabaseManager::instance().close();
    m_initialized = false;
}

void ApiBridge::rebuildIndex() {
    if (m_bookService) {
        auto books = m_bookService->getAllBooks();
        m_searchIndex->buildIndex(books);
    }

    // 閲嶅缓鎺ㄨ崘寮曟搸鐭╅樀
    if (m_borrowService) {
        QSqlDatabase db = DatabaseManager::instance().database();
        QSqlQuery query(db);
        query.exec("SELECT * FROM borrow_records");
        QVector<BorrowRecord> records;
        while (query.next()) {
            BorrowRecord r;
            r.id = query.value("id").toInt();
            r.userId = query.value("user_id").toInt();
            r.bookId = query.value("book_id").toInt();
            records.append(r);
        }
        m_recommendEngine->buildMatrix(records);
    }

    // 閲嶅缓鍒嗙被鏍?
    if (m_bookService) {
        QSqlDatabase db = DatabaseManager::instance().database();
        QSqlQuery query(db);
        query.exec("SELECT * FROM categories");
        QVector<Category> categories;
        while (query.next()) {
            Category c;
            c.id = query.value("id").toInt();
            c.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
            c.name = query.value("name").toString();
            c.path = query.value("path").toString();
            c.level = query.value("level").toInt();
            categories.append(c);
        }
        m_categoryTree->build(categories);
    }
}

// ============ 璁よ瘉 ============

Result ApiBridge::login(const QString& username, const QString& password) {
    Result result = m_authService->login(username, password);
    if (result.success) {
        m_currentUser = m_authService->currentUser();
        emit loggedIn(m_currentUser);
    }
    return result;
}
Result ApiBridge::registerUser(const QString& username, const QString& password,
                                const QString& displayName, UserRole role) {
    return m_authService->registerUser(username, password, displayName, role);
}

void ApiBridge::logout() {
    m_authService->logout();
    emit loggedOut();
    m_currentUser = User{};
}

// ============ 妫€绱?============

QVector<Book> ApiBridge::searchBooks(const QString& keywords, int topN) {
    return m_searchIndex->search(keywords, topN);
}

Book ApiBridge::getBookDetail(int bookId) {
    return m_bookService->getBookById(bookId);
}

QVector<Book> ApiBridge::getRecommendations(int userId, int topN) {
    auto allBooks = m_bookService->getAllBooks();
    return m_recommendEngine->getRecommendations(userId, allBooks, topN);
}

QVector<Book> ApiBridge::getHotBooks(int topN) {
    auto allBooks = m_bookService->getAllBooks();
    return m_recommendEngine->getHotRecommendations(allBooks, topN);
}

// ============ 鍊熼槄 ============

Result ApiBridge::borrowBook(int userId, int bookId) {
    Result result = m_borrowService->borrowBook(userId, bookId);
    if (result.success) {
        // 鏇存柊绱㈠紩
        auto book = m_bookService->getBookById(bookId);
        m_searchIndex->updateBook(book);
        rebuildIndex();  // 同时也重建推荐矩阵
        emit borrowSucceeded(result.message);
    }
    return result;
}

Result ApiBridge::returnBook(int recordId) {
    // 鑾峰彇bookId缁欓€氱煡鐢?
    BorrowRecord record;
    {
        QSqlQuery query(DatabaseManager::instance().database());
        query.prepare("SELECT book_id, user_id FROM borrow_records WHERE id = ?");
        query.addBindValue(recordId);
        if (query.exec() && query.next()) {
            record.bookId = query.value("book_id").toInt();
            if (recordId != 0) {
                record.id = recordId;
            }
        }
    }

    Result result = m_borrowService->returnBook(recordId);
    if (result.success) {
        // 鏇存柊绱㈠紩
        if (record.bookId > 0) {
            auto book = m_bookService->getBookById(record.bookId);
            m_searchIndex->updateBook(book);
            rebuildIndex();  // 同时也重建推荐矩阵
        }
        // 妫€鏌ラ绾﹂槦鍒?
        if (record.bookId > 0) {
            int nextReservationId = m_reservationService->notifyNextInQueue(record.bookId);
            if (nextReservationId > 0) {
                qInfo() << "Notifying reservation" << nextReservationId << "for book" << record.bookId;
            }
        }
        emit returnSucceeded(result.message);
    }
    return result;
}

Result ApiBridge::renewBook(int recordId) {
    return m_borrowService->renewBook(recordId);
}

QVector<BorrowRecord> ApiBridge::getMyBorrowRecords(int userId) const {
    return m_borrowService->getUserRecords(userId);
}

QVector<BorrowRecord> ApiBridge::getMyActiveRecords(int userId) const {
    return m_borrowService->getUserActiveRecords(userId);
}

// ============ 棰勭害 ============

Result ApiBridge::reserveBook(int userId, int bookId) {
    return m_reservationService->reserveBook(userId, bookId);
}

Result ApiBridge::deleteReservationRecord(int reservationId) {
    return m_reservationService->deleteReservation(reservationId);
}
Result ApiBridge::cancelReservation(int reservationId) {
    return m_reservationService->cancelReservation(reservationId);
}
int ApiBridge::deleteAllCancelledRecords() {
    return m_reservationService->deleteAllCancelled();
}


QVector<Reservation> ApiBridge::getMyReservations(int userId) const {
    return m_reservationService->getUserReservations(userId);
}

// ============ 绠＄悊 ============

Result ApiBridge::addBook(const Book& book) {
    Result result = m_bookService->addBook(book);
    if (result.success) {
        // 閲嶅缓绱㈠紩
        rebuildIndex();
        emit bookAdded(book);
    }
    return result;
}

Result ApiBridge::updateBook(const Book& book) {
    Result result = m_bookService->updateBook(book);
    if (result.success) {
        rebuildIndex();
        emit bookUpdated(book);
    }
    return result;
}

Result ApiBridge::deleteBook(int bookId) {
    Result result = m_bookService->deleteBook(bookId);
    if (result.success) {
        m_searchIndex->removeBook(bookId);
        emit bookDeleted(bookId);
    }
    return result;
}

QVector<Book> ApiBridge::getAllBooks() const {
    return m_bookService->getAllBooks();
}

#include "dao/user_dao.h"
QVector<User> ApiBridge::getAllUsers() const {
    UserDAO dao(DatabaseManager::instance().database());
    return dao.getAll();
}

Result ApiBridge::disableUser(int userId, bool disabled) {
    UserDAO dao(DatabaseManager::instance().database());
    if (dao.setDisabled(userId, disabled)) {
        return Result::ok(disabled ? "已禁用用户" : "已启用用户");
    }
    return Result::fail("鎿嶄綔澶辫触");
}

// ============ 缁熻 ============

Statistics ApiBridge::getStatistics() {
    Statistics stats;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    // 鎬诲浘涔︽暟
    query.exec("SELECT COUNT(*) FROM books");
    if (query.next()) stats.totalBooks = query.value(0).toInt();

    // 鎬荤敤鎴锋暟
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next()) stats.totalUsers = query.value(0).toInt();

    // 褰撳墠鍊熼槄涓?
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE status = 0");
    if (query.next()) stats.activeBorrows = query.value(0).toInt();

    // 閫炬湡鏁伴噺
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE status = 0 AND due_date < datetime('now','localtime')");
    if (query.next()) stats.overdueCount = query.value(0).toInt();

    // 鎬荤綒娆?
    query.exec("SELECT COALESCE(SUM(fine), 0) FROM borrow_records");
    if (query.next()) stats.totalFine = query.value(0).toDouble();

    // 浠婃棩鍊熼槄
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE date(borrow_date) = date('now','localtime')");
    if (query.next()) stats.todayBorrows = query.value(0).toInt();

    // 浠婃棩褰掕繕
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE date(return_date) = date('now','localtime')");
    if (query.next()) stats.todayReturns = query.value(0).toInt();

    // 鏈堝害缁熻锛堟渶杩?涓湀锛?
    query.exec(
        "SELECT strftime('%Y%m', borrow_date) as month, COUNT(*) as count "
        "FROM borrow_records "
        "WHERE borrow_date >= date('now','-6 months') "
        "GROUP BY month ORDER BY month"
    );
    while (query.next()) {
        Statistics::MonthlyStat ms;
        ms.month = query.value("month").toString().toInt();
        ms.borrowCount = query.value("count").toInt();
        stats.monthlyStats.append(ms);
    }

    // 鍒嗙被缁熻
    query.exec(
        "SELECT c.id, c.name, COUNT(b.id) as count "
        "FROM categories c LEFT JOIN books b ON c.id = b.category_id "
        "GROUP BY c.id ORDER BY count DESC"
    );
    while (query.next()) {
        Statistics::CategoryStat cs;
        cs.categoryId = query.value("id").toInt();
        cs.categoryName = query.value("name").toString();
        cs.bookCount = query.value("count").toInt();
        stats.categoryStats.append(cs);
    }

    // 鐑棬鍥句功TOP10
    query.exec(
        "SELECT b.id, b.title, COUNT(br.id) as borrow_count "
        "FROM books b JOIN borrow_records br ON b.id = br.book_id "
        "GROUP BY b.id ORDER BY borrow_count DESC LIMIT 10"
    );
    while (query.next()) {
        Statistics::HotBook hb;
        hb.bookId = query.value("id").toInt();
        hb.bookTitle = query.value("title").toString();
        hb.borrowCount = query.value("borrow_count").toInt();
        stats.hotBooks.append(hb);
    }

    return stats;
}

QVector<Book> ApiBridge::getBooksByCategory(int categoryId) const {
    return m_bookService->getBooksByCategory(categoryId);
}

QVector<Category> ApiBridge::getAllCategories() const {
    return m_categoryTree ? m_categoryTree->allCategories() : QVector<Category>();
}

QVector<Category> ApiBridge::getCategoryChildren(int parentId) const {
    return m_categoryTree ? m_categoryTree->getChildren(parentId) : QVector<Category>();
}

QVector<Category> ApiBridge::getRootCategories() const {
    return m_categoryTree ? m_categoryTree->getRootCategories() : QVector<Category>();
}

QSet<int> ApiBridge::getDescendantCategoryIds(int categoryId) const {
    return m_categoryTree ? m_categoryTree->getDescendantIds(categoryId) : QSet<int>();
}

QVector<LogEntry> ApiBridge::getRecentLogs() const {
    return m_logService ? m_logService->getRecentLogs() : QVector<LogEntry>();
}