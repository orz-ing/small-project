#include "backend.h"
#include <sqlite3.h>
BorrowRecordDao::BorrowRecordDao(DatabaseManager& db) : db_(db) {}
BorrowRecord BorrowRecordDao::rowToRecord(void* stmt) const {
    auto* s = (sqlite3_stmt*)stmt; BorrowRecord r;
    r.setId(sqlite3_column_int(s,0)); r.setUserId(sqlite3_column_int(s,1));
    r.setBookId(sqlite3_column_int(s,2));
    r.setBorrowDate((const char*)sqlite3_column_text(s,3));
    r.setDueDate((const char*)sqlite3_column_text(s,4));
    r.setReturnDate(sqlite3_column_text(s,5)?(const char*)sqlite3_column_text(s,5):"");
    r.setStatus((const char*)sqlite3_column_text(s,6));
    r.setRenewCount(sqlite3_column_int(s,7)); return r;
}
BorrowRecord BorrowRecordDao::insert(const BorrowRecord& r, Status& status) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "INSERT INTO borrow_records (user_id,book_id,borrow_date,due_date,status) VALUES (?,?,?,?,?)";
    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK){status={StatusCode::ERR_DB_EXEC,"prepare failed"};return BorrowRecord();}
    sqlite3_bind_int(stmt,1,r.getUserId()); sqlite3_bind_int(stmt,2,r.getBookId());
    sqlite3_bind_text(stmt,3,r.getBorrowDate().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,4,r.getDueDate().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,5,r.getStatus().c_str(),-1,SQLITE_STATIC);
    if(sqlite3_step(stmt)!=SQLITE_DONE){status={StatusCode::ERR_DB_EXEC,"insert failed"};sqlite3_finalize(stmt);return BorrowRecord();}
    int id=(int)sqlite3_last_insert_rowid(db); sqlite3_finalize(stmt);
    BorrowRecord created=r; created.setId(id); status=StatusOK(); return created;
}
vector<BorrowRecord> BorrowRecordDao::findByUser(int userId){
    auto* db=(sqlite3*)db_.getHandle(); vector<BorrowRecord> recs;
    const char* sql="SELECT * FROM borrow_records WHERE user_id=? ORDER BY id DESC";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return recs;
    sqlite3_bind_int(stmt,1,userId);
    while(sqlite3_step(stmt)==SQLITE_ROW) recs.push_back(rowToRecord(stmt));
    sqlite3_finalize(stmt); return recs;
}
vector<BorrowRecord> BorrowRecordDao::findOverdue(){
    auto* db=(sqlite3*)db_.getHandle(); vector<BorrowRecord> recs;
    const char* sql="SELECT * FROM borrow_records WHERE status='borrowing' AND due_date<datetime('now','localtime') ORDER BY id";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return recs;
    while(sqlite3_step(stmt)==SQLITE_ROW) recs.push_back(rowToRecord(stmt));
    sqlite3_finalize(stmt); return recs;
}
Status BorrowRecordDao::updateReturn(int borrowId, const string& returnDate){
    return db_.execute("UPDATE borrow_records SET return_date='"+returnDate+"',status='returned' WHERE id="+to_string(borrowId));
}
Status BorrowRecordDao::updateRenew(int borrowId, const string& newDueDate){
    return db_.execute("UPDATE borrow_records SET due_date='"+newDueDate+"',renew_count=renew_count+1 WHERE id="+to_string(borrowId));
}
Status BorrowRecordDao::updateStatus(int borrowId, const string& status){
    return db_.execute("UPDATE borrow_records SET status='"+status+"' WHERE id="+to_string(borrowId));
}
