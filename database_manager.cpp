// database_manager.cpp — SQLite 数据库管理器
#include "backend.h"
using namespace std;

DatabaseManager::DatabaseManager() : db_(nullptr) {}
DatabaseManager::~DatabaseManager() { close(); }

// ===== 打开数据库 =====
// 输入: dbPath(文件路径，默认 "library.db")
// 输出: true=打开成功并建表; false=打开失败
bool DatabaseManager::open(const string& dbPath)
{
    // TODO: sqlite3_open → createTables() → createIndexes() → createTriggers()
}

void DatabaseManager::close()
{
    // TODO: sqlite3_close
}

// ===== 建表 =====
// 创建6张表: users, books, borrow_records, reservations, categories, log_entries
void DatabaseManager::createTables()
{
    // TODO: CREATE TABLE IF NOT EXISTS users (...)
}

// ===== 建索引 =====
void DatabaseManager::createIndexes()
{
    // TODO: CREATE INDEX idx_users_username ON users(username)
}

// ===== 建触发器 =====
void DatabaseManager::createTriggers()
{
    // TODO: CREATE TRIGGER borrow_insert AFTER INSERT ON borrow_records
    //       BEGIN UPDATE books SET stock=stock-1 WHERE id=NEW.book_id; END;
}

// ===== 事务 =====
bool DatabaseManager::beginTransaction() { /* TODO: sqlite3_exec("BEGIN") */ }
bool DatabaseManager::commit()           { /* TODO: sqlite3_exec("COMMIT") */ }
bool DatabaseManager::rollback()         { /* TODO: sqlite3_exec("ROLLBACK") */ }

// ===== 执行SQL =====
// 输入: SQL 语句（字符串）
// 输出: true=执行成功; false=失败
bool DatabaseManager::execute(const string& sql)
{
    // TODO: sqlite3_exec
}

// 输出: sqlite3* 原生句柄
void* DatabaseManager::getHandle()
{
    return db_;
}
