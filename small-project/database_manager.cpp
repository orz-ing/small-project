#include "backend.h"
#include <sqlite3.h>
#include <stdexcept>
DatabaseManager::DatabaseManager() : db_(nullptr) {}
DatabaseManager::~DatabaseManager() { close(); }
Status DatabaseManager::open(const string& dbPath) {
    if (db_) close();
    int rc = sqlite3_open(dbPath.c_str(), (sqlite3**)&db_);
    if (rc != SQLITE_OK) {
        close(); return {StatusCode::ERR_DB_OPEN, sqlite3_errmsg((sqlite3*)db_)};
    }
    sqlite3_exec((sqlite3*)db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec((sqlite3*)db_, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr);
    try { createTables(); createIndexes(); createTriggers(); }
    catch (exception& e) { return {StatusCode::ERR_DB_EXEC, e.what()}; }
    return StatusOK();
}
void DatabaseManager::close() { if (db_) { sqlite3_close((sqlite3*)db_); db_ = nullptr; } }
Status DatabaseManager::beginTransaction() { return execute("BEGIN"); }
Status DatabaseManager::commit() { return execute("COMMIT"); }
Status DatabaseManager::rollback() { return execute("ROLLBACK"); }
Status DatabaseManager::execute(const string& sql) {
    if (!db_) return {StatusCode::ERR_DB_OPEN, "db not open"};
    char* err = nullptr;
    if (sqlite3_exec((sqlite3*)db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        string msg = err ? err : "exec failed";
        sqlite3_free(err); return {StatusCode::ERR_DB_EXEC, msg};
    }
    return StatusOK();
}
void* DatabaseManager::getHandle() { return db_; }
void DatabaseManager::createTables() {
    string sql = R"(CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT,username TEXT UNIQUE NOT NULL,password_hash TEXT NOT NULL,real_name TEXT NOT NULL,role TEXT NOT NULL DEFAULT 'reader',credit INTEGER NOT NULL DEFAULT 100,status TEXT NOT NULL DEFAULT 'active',phone TEXT DEFAULT '',email TEXT DEFAULT '',create_time TEXT NOT NULL DEFAULT (datetime('now','localtime')));)";
    sql += R"(CREATE TABLE IF NOT EXISTS categories (id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL,parent_id INTEGER NOT NULL DEFAULT 0,level INTEGER NOT NULL DEFAULT 0);)";
    sql += R"(CREATE TABLE IF NOT EXISTS books (id INTEGER PRIMARY KEY AUTOINCREMENT,isbn TEXT UNIQUE,title TEXT NOT NULL,author TEXT NOT NULL DEFAULT '',publisher TEXT DEFAULT '',publish_date TEXT DEFAULT '',category_id INTEGER DEFAULT 0,stock INTEGER NOT NULL DEFAULT 0,total_stock INTEGER NOT NULL DEFAULT 0,version INTEGER NOT NULL DEFAULT 1,location TEXT DEFAULT '',description TEXT DEFAULT '',cover_url TEXT DEFAULT '');)";
    sql += R"(CREATE TABLE IF NOT EXISTS borrow_records (id INTEGER PRIMARY KEY AUTOINCREMENT,user_id INTEGER NOT NULL,book_id INTEGER NOT NULL,borrow_date TEXT NOT NULL,due_date TEXT NOT NULL,return_date TEXT DEFAULT '',status TEXT NOT NULL DEFAULT 'borrowing',renew_count INTEGER NOT NULL DEFAULT 0,FOREIGN KEY (user_id) REFERENCES users(id),FOREIGN KEY (book_id) REFERENCES books(id));)";
    sql += R"(CREATE TABLE IF NOT EXISTS reservations (id INTEGER PRIMARY KEY AUTOINCREMENT,user_id INTEGER NOT NULL,book_id INTEGER NOT NULL,reserve_date TEXT NOT NULL,expire_date TEXT NOT NULL,status TEXT NOT NULL DEFAULT 'pending',priority INTEGER NOT NULL DEFAULT 0,FOREIGN KEY (user_id) REFERENCES users(id),FOREIGN KEY (book_id) REFERENCES books(id));)";
    sql += R"(CREATE TABLE IF NOT EXISTS log_entries (id INTEGER PRIMARY KEY AUTOINCREMENT,admin_id INTEGER NOT NULL DEFAULT 0,action TEXT NOT NULL,target TEXT DEFAULT '',detail TEXT DEFAULT '',time TEXT NOT NULL DEFAULT (datetime('now','localtime')));)";
    char* e=nullptr; if(sqlite3_exec((sqlite3*)db_,sql.c_str(),nullptr,nullptr,&e)!=SQLITE_OK){string m=e?e:"failed";sqlite3_free(e);throw runtime_error(m);}
}
void DatabaseManager::createIndexes() {
    sqlite3_exec((sqlite3*)db_,"CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);CREATE INDEX IF NOT EXISTS idx_books_category ON books(category_id);CREATE INDEX IF NOT EXISTS idx_borrow_user ON borrow_records(user_id);CREATE INDEX IF NOT EXISTS idx_borrow_book ON borrow_records(book_id);CREATE INDEX IF NOT EXISTS idx_borrow_status ON borrow_records(status);CREATE INDEX IF NOT EXISTS idx_reservation_book ON reservations(book_id);CREATE INDEX IF NOT EXISTS idx_reservation_status ON reservations(status);CREATE INDEX IF NOT EXISTS idx_log_time ON log_entries(time);",nullptr,nullptr,nullptr);
}
void DatabaseManager::createTriggers() {
    sqlite3_exec((sqlite3*)db_,"CREATE TRIGGER IF NOT EXISTS trg_borrow_insert AFTER INSERT ON borrow_records BEGIN UPDATE books SET stock=stock-1 WHERE id=NEW.book_id; END;",nullptr,nullptr,nullptr);
    sqlite3_exec((sqlite3*)db_,"CREATE TRIGGER IF NOT EXISTS trg_borrow_return AFTER UPDATE OF status ON borrow_records WHEN NEW.status='returned' BEGIN UPDATE books SET stock=stock+1 WHERE id=NEW.book_id; END;",nullptr,nullptr,nullptr);
}
