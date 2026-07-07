// borrow_record_dao.cpp — 借阅记录数据访问对象
#include "backend.h"
using namespace std;

BorrowRecordDao::BorrowRecordDao(DatabaseManager& db) : db_(db) {}

// 输入: BorrowRecord 对象（id 忽略）
// 输出: 新插入记录的 id
int BorrowRecordDao::insert(const BorrowRecord& r)
{
    // TODO: INSERT INTO borrow_records(...) VALUES(...)
}

// 输入: userId
// 输出: 该用户的全部借阅记录（按借阅日期倒序）
vector<BorrowRecord> BorrowRecordDao::findByUser(int userId)
{
    // TODO: SELECT * FROM borrow_records WHERE user_id=? ORDER BY borrow_date DESC
}

// 输出: 所有逾期且未归还的记录（due_date < today AND status='borrowing'）
vector<BorrowRecord> BorrowRecordDao::findOverdue()
{
    // TODO: SELECT * FROM borrow_records WHERE due_date < date('now') AND status='borrowing'
}

// 输入: borrowId, returnDate("YYYY-MM-DD")
// 输出: true=更新成功
bool BorrowRecordDao::updateReturn(int borrowId, const string& returnDate)
{
    // TODO: UPDATE borrow_records SET return_date=?, status='returned' WHERE id=?
}

// 输入: borrowId, newDueDate("YYYY-MM-DD")
// 输出: true=更新成功
bool BorrowRecordDao::updateRenew(int borrowId, const string& newDueDate)
{
    // TODO: UPDATE borrow_records SET due_date=?, renew_count=renew_count+1 WHERE id=?
}

// 输入: borrowId, status("borrowing"/"returned"/"overdue")
// 输出: true=更新成功
bool BorrowRecordDao::updateStatus(int borrowId, const string& status)
{
    // TODO: UPDATE borrow_records SET status=? WHERE id=?
}

BorrowRecord BorrowRecordDao::rowToRecord(void* stmt) const
{
    // TODO: sqlite3 行映射
}
