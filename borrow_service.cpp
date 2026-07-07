// borrow_service.cpp — 借阅服务（核心：乐观锁事务）
#include "backend.h"
using namespace std;

BorrowService::BorrowService(BorrowRecordDao* brDao, BookDao* bDao,
    CreditEngine* credit, ReservationQueue* queue)
    : borrowDao_(brDao), bookDao_(bDao), creditEng_(credit), resQueue_(queue) {}

// ===== 借书（乐观锁核心）=====
// 输入: userId, bookId
// 输出: ""=借阅成功
//        "库存不足"=stock≤0
//        "信用分不足"=信用<60
//        "乐观锁冲突"=重试3次后仍失败
string BorrowService::borrowBook(int userId, int bookId)
{
    // TODO: [乐观锁事务循环]
    // for retry 0..MAX_RETRY:
    //   BEGIN TRANSACTION
    //   1. Book book = bookDao_->findById(bookId)
    //   2. if !creditEng_->canBorrow(userCredit) → 回滚 → 返回"信用分不足"
    //   3. if !book.isAvailable() → 回滚 → 返回"库存不足"
    //   4. ok = bookDao_->updateStockWithLock(bookId, -1, book.getVersion())
    //   5. if !ok → 回滚 → continue (版本冲突, 重试)
    //   6. borrowDao_->insert(record)
    //   COMMIT → 返回""
    // 返回 "乐观锁冲突"
}

// ===== 还书 =====
// 输入: borrowId
// 输出: ""=成功; "记录不存在"=无效ID
string BorrowService::returnBook(int borrowId)
{
    // TODO:
    // 1. 查询借阅记录 → 标记 returnDate → 改状态 "returned"
    // 2. bookDao_->updateStockWithLock(bookId, +1, ...)
    // 3. 计算罚款（若逾期）：FineCalculator::calcFine()
    // 4. 更新信用分：CreditEngine::updateScore()
    // 5. resQueue_->dequeue(bookId) — 通知预约队首
}

// ===== 续借 =====
// 输入: borrowId
// 输出: 新的截止日期字符串; 空串=续借失败（已达上限/已逾期）
string BorrowService::renewBook(int borrowId)
{
    // TODO: 检查 renewCount < MAX_RENEW 且未逾期 → dueDate += RENEW_DAYS
}

// ===== 我的借阅记录 =====
// 输入: userId
// 输出: 该用户的全部借阅记录
vector<BorrowRecord> BorrowService::getMyRecords(int userId)
{
    // TODO: borrowDao_->findByUser(userId)
}
