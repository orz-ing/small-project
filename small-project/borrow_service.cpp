#include "backend.h"
#include "utils.h"
BorrowService::BorrowService(BorrowRecordDao* brDao, BookDao* bDao,
                              CreditEngine* credit, ReservationQueue* queue)
    : borrowDao_(brDao), bookDao_(bDao), creditEng_(credit), resQueue_(queue) {}

BorrowRecord BorrowService::borrowBook(int userId, int bookId, Status& status, bool& done) {
    done = false;
    auto book = bookDao_->findById(bookId);
    if (book.getId() == 0) { done = true; status = {StatusCode::ERR_BOOK_NOT_FOUND, "book not found"}; return BorrowRecord(); }
    if (book.getStock() <= 0) { done = true; status = {StatusCode::ERR_BOOK_OUT_OF_STOCK, "out of stock"}; return BorrowRecord(); }
    Status lockStatus = bookDao_->updateStockWithLock(bookId, -1, book.getVersion());
    if (!lockStatus.ok()) {
        if (lockStatus.code == StatusCode::ERR_VERSION_CONFLICT) {
            status = {StatusCode::ERR_VERSION_CONFLICT, "stock conflict, retry"};
            done = false; return BorrowRecord();
        }
        done = true; status = lockStatus; return BorrowRecord();
    }
    string borrowDate = today();
    string dueDate = addDays(borrowDate, BORROW_DAYS);
    BorrowRecord record;
    record.setUserId(userId); record.setBookId(bookId);
    record.setBorrowDate(borrowDate); record.setDueDate(dueDate);
    record.setStatus("borrowing");
    Status s2; BorrowRecord created = borrowDao_->insert(record, s2);
    if (!s2.ok()) {
        bookDao_->updateStockWithLock(bookId, 1, book.getVersion() + 1);
        done = true; status = s2; return BorrowRecord();
    }
    done = true; status = StatusOK(); return created;
}

BorrowRecord BorrowService::returnBook(int borrowId, Status& status, bool& done) {
    // Full flow with DatabaseManager access is in ApiBridge::returnBook
    done = true;
    status = {StatusCode::ERR_BORROW_NOT_FOUND, "use ApiBridge::returnBook"};
    return BorrowRecord();
}

BorrowRecord BorrowService::renewBook(int borrowId, Status& status, bool& done) {
    done = true;
    status = {StatusCode::ERR_BORROW_NOT_FOUND, "use ApiBridge::renewBook"};
    return BorrowRecord();
}

vector<BorrowRecord> BorrowService::getMyRecords(int userId) {
    return borrowDao_->findByUser(userId);
}
