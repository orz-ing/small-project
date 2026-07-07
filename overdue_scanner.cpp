// overdue_scanner.cpp — 逾期扫描器（QTimer 定时任务，每日 00:00）
#include "backend.h"
using namespace std;

OverdueScanner::OverdueScanner(BorrowRecordDao* brDao, BookDao* bDao,
    CreditEngine* credit)
    : borrowDao_(brDao), bookDao_(bDao), creditEng_(credit) {}

// ===== 执行一次扫描 =====
// 无输入
// 副作用: 更新逾期记录状态 → 计算罚款 → 扣信用分
void OverdueScanner::scan()
{
    // TODO:
    // 1. vector<BorrowRecord> overdue = borrowDao_->findOverdue()
    // 2. 未逾期的标记为 "overdue" 状态
    // 3. 对每条逾期记录:
    //    - FineCalculator::calcFine(dueDate, today)
    //    - creditEng_->updateScore(score, "overdue")
    //    - borrowDao_->updateStatus(id, "overdue")
}
