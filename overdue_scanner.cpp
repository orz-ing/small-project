#include "backend.h"
#include "utils.h"

OverdueScanner::OverdueScanner(BorrowRecordDao* brDao, BookDao* bDao, CreditEngine* credit)
    : borrowDao_(brDao), bookDao_(bDao), creditEng_(credit) {}

void OverdueScanner::scan() {
    auto overdue = borrowDao_->findOverdue();
    for (auto& r : overdue) {
        borrowDao_->updateStatus(r.getId(), "overdue");
        // Deduct credit for overdue
        double fine = FineCalculator::calcFine(r.getDueDate(), today());
        int newCredit = CreditEngine::updateScore(r.getUserId() > 0 ? 100 : 100, "overdue");
        // Note: credit update requires UserDao access, which we don"t have here
        // This is handled in ApiBridge::returnBook
    }
}
