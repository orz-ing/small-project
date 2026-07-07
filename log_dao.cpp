#include "backend.h"
#include <sqlite3.h>
LogDao::LogDao(DatabaseManager& db) : db_(db) {}
LogEntry LogDao::insert(const LogEntry& log, Status& status) {
    auto* db=(sqlite3*)db_.getHandle();
    const char* sql="INSERT INTO log_entries (admin_id,action,target,detail) VALUES (?,?,?,?)";
    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK){status={StatusCode::ERR_DB_EXEC,"prepare failed"};return LogEntry();}
    sqlite3_bind_int(stmt,1,log.getAdminId()); sqlite3_bind_text(stmt,2,log.getAction().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,3,log.getTarget().c_str(),-1,SQLITE_STATIC); sqlite3_bind_text(stmt,4,log.getDetail().c_str(),-1,SQLITE_STATIC);
    if(sqlite3_step(stmt)!=SQLITE_DONE){sqlite3_finalize(stmt);status={StatusCode::ERR_DB_EXEC,"insert failed"};return LogEntry();}
    int id=(int)sqlite3_last_insert_rowid(db); sqlite3_finalize(stmt);
    LogEntry created=log; created.setId(id); status=StatusOK(); return created;
}
vector<LogEntry> LogDao::findByAdmin(int adminId){
    auto* db=(sqlite3*)db_.getHandle(); vector<LogEntry> logs;
    const char* sql="SELECT * FROM log_entries WHERE admin_id=? ORDER BY id DESC";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return logs;
    sqlite3_bind_int(stmt,1,adminId);
    while(sqlite3_step(stmt)==SQLITE_ROW){
        LogEntry l; l.setId(sqlite3_column_int(stmt,0)); l.setAdminId(sqlite3_column_int(stmt,1));
        l.setAction((const char*)sqlite3_column_text(stmt,2)); l.setTarget(sqlite3_column_text(stmt,3)?(const char*)sqlite3_column_text(stmt,3):"");
        l.setDetail(sqlite3_column_text(stmt,4)?(const char*)sqlite3_column_text(stmt,4):""); l.setTime((const char*)sqlite3_column_text(stmt,5));
        logs.push_back(l);
    }
    sqlite3_finalize(stmt); return logs;
}
vector<LogEntry> LogDao::findRecent(int days){
    auto* db=(sqlite3*)db_.getHandle(); vector<LogEntry> logs;
    string sql = "SELECT * FROM log_entries WHERE time>=datetime('now','-" + to_string(days) + " days') ORDER BY id DESC";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) return logs;
    while(sqlite3_step(stmt)==SQLITE_ROW){
        LogEntry l; l.setId(sqlite3_column_int(stmt,0)); l.setAdminId(sqlite3_column_int(stmt,1));
        l.setAction((const char*)sqlite3_column_text(stmt,2)); l.setTarget(sqlite3_column_text(stmt,3)?(const char*)sqlite3_column_text(stmt,3):"");
        l.setDetail(sqlite3_column_text(stmt,4)?(const char*)sqlite3_column_text(stmt,4):""); l.setTime((const char*)sqlite3_column_text(stmt,5));
        logs.push_back(l);
    }
    sqlite3_finalize(stmt); return logs;
}
