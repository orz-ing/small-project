#include "backend.h"
#include <sqlite3.h>
ReservationDao::ReservationDao(DatabaseManager& db) : db_(db) {}
Reservation ReservationDao::rowToReservation(void* stmt) const {
    auto* s = (sqlite3_stmt*)stmt; Reservation r;
    r.setId(sqlite3_column_int(s,0)); r.setUserId(sqlite3_column_int(s,1)); r.setBookId(sqlite3_column_int(s,2));
    r.setReserveDate((const char*)sqlite3_column_text(s,3)); r.setExpireDate((const char*)sqlite3_column_text(s,4));
    r.setStatus((const char*)sqlite3_column_text(s,5)); r.setPriority(sqlite3_column_int(s,6)); return r;
}
Reservation ReservationDao::insert(const Reservation& r, Status& status) {
    auto* db=(sqlite3*)db_.getHandle();
    const char* sql="INSERT INTO reservations (user_id,book_id,reserve_date,expire_date,status,priority) VALUES (?,?,?,?,?,?)";
    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK){status={StatusCode::ERR_DB_EXEC,"prepare failed"};return Reservation();}
    sqlite3_bind_int(stmt,1,r.getUserId()); sqlite3_bind_int(stmt,2,r.getBookId());
    sqlite3_bind_text(stmt,3,r.getReserveDate().c_str(),-1,SQLITE_STATIC); sqlite3_bind_text(stmt,4,r.getExpireDate().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,5,r.getStatus().c_str(),-1,SQLITE_STATIC); sqlite3_bind_int(stmt,6,r.getPriority());
    if(sqlite3_step(stmt)!=SQLITE_DONE){sqlite3_finalize(stmt);status={StatusCode::ERR_DB_EXEC,"insert failed"};return Reservation();}
    int id=(int)sqlite3_last_insert_rowid(db); sqlite3_finalize(stmt);
    Reservation cr=r; cr.setId(id); status=StatusOK(); return cr;
}
vector<Reservation> ReservationDao::findByUser(int userId){
    auto* db=(sqlite3*)db_.getHandle(); vector<Reservation> res;
    const char* sql="SELECT * FROM reservations WHERE user_id=? ORDER BY id DESC";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return res;
    sqlite3_bind_int(stmt,1,userId);
    while(sqlite3_step(stmt)==SQLITE_ROW) res.push_back(rowToReservation(stmt));
    sqlite3_finalize(stmt); return res;
}
vector<Reservation> ReservationDao::findByBook(int bookId){
    auto* db=(sqlite3*)db_.getHandle(); vector<Reservation> res;
    const char* sql="SELECT * FROM reservations WHERE book_id=? AND status='pending' ORDER BY priority DESC, id";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return res;
    sqlite3_bind_int(stmt,1,bookId);
    while(sqlite3_step(stmt)==SQLITE_ROW) res.push_back(rowToReservation(stmt));
    sqlite3_finalize(stmt); return res;
}
Status ReservationDao::updateStatus(int reservationId, const string& status){
    return db_.execute("UPDATE reservations SET status='"+status+"' WHERE id="+to_string(reservationId));
}
vector<Reservation> ReservationDao::findExpired(int timeoutHours){
    auto* db=(sqlite3*)db_.getHandle(); vector<Reservation> res;
    string sql="SELECT * FROM reservations WHERE status='pending' AND expire_date<datetime('now','localtime')";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) return res;
    while(sqlite3_step(stmt)==SQLITE_ROW) res.push_back(rowToReservation(stmt));
    sqlite3_finalize(stmt); return res;
}
