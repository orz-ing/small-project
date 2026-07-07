#include "backend.h"
#include <sqlite3.h>

UserDao::UserDao(DatabaseManager& db) : db_(db) {}

User UserDao::rowToUser(void* stmt) const {
    auto* s = (sqlite3_stmt*)stmt;
    User u;
    u.setId(sqlite3_column_int(s, 0));
    u.setUsername((const char*)sqlite3_column_text(s, 1));
    u.setPasswordHash((const char*)sqlite3_column_text(s, 2));
    u.setRealName((const char*)sqlite3_column_text(s, 3));
    string role = (const char*)sqlite3_column_text(s, 4);
    // Use setStatus for validation
    u.setCredit(sqlite3_column_int(s, 5));
    string status = (const char*)sqlite3_column_text(s, 6);
    u.setStatus(status);
    string phone = sqlite3_column_text(s, 7) ? (const char*)sqlite3_column_text(s, 7) : "";
    u.setPhone(phone);
    string email = sqlite3_column_text(s, 8) ? (const char*)sqlite3_column_text(s, 8) : "";
    u.setEmail(email);
    string ct = (const char*)sqlite3_column_text(s, 9);
    u.setCreateTime(ct);
    return u;
}

User UserDao::findByUsername(const string& username) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "SELECT * FROM users WHERE username=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return User();
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    User u;
    if (sqlite3_step(stmt) == SQLITE_ROW) u = rowToUser(stmt);
    sqlite3_finalize(stmt);
    return u;
}

User UserDao::findById(int userId) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "SELECT * FROM users WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return User();
    sqlite3_bind_int(stmt, 1, userId);
    User u;
    if (sqlite3_step(stmt) == SQLITE_ROW) u = rowToUser(stmt);
    sqlite3_finalize(stmt);
    return u;
}

vector<User> UserDao::findAll() {
    auto* db = (sqlite3*)db_.getHandle();
    vector<User> users;
    const char* sql = "SELECT * FROM users ORDER BY id";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return users;
    while (sqlite3_step(stmt) == SQLITE_ROW) users.push_back(rowToUser(stmt));
    sqlite3_finalize(stmt);
    return users;
}

User UserDao::insert(const User& user, Status& status) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "INSERT INTO users (username,password_hash,real_name,role,credit,status,phone,email) VALUES (?,?,?,?,?,?,?,?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        status = {StatusCode::ERR_DB_EXEC, "prepare insert user failed"};
        return User();
    }
    sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.getPasswordHash().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user.getRealName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, "reader", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, user.getCredit());
    sqlite3_bind_text(stmt, 6, user.getStatus().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, user.getPhone().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, user.getEmail().c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        status = {StatusCode::ERR_USER_EXISTS, "username already exists"};
        sqlite3_finalize(stmt);
        return User();
    }
    int id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    User created = user;
    created.setId(id);
    status = StatusOK();
    return created;
}

Status UserDao::update(const User& user) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "UPDATE users SET real_name=?,phone=?,email=? WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {StatusCode::ERR_DB_EXEC, "prepare update user failed"};
    sqlite3_bind_text(stmt, 1, user.getRealName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.getPhone().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user.getEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, user.getId());
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? StatusOK() : Status{StatusCode::ERR_UNKNOWN, "update user failed"};
}

Status UserDao::updateCredit(int userId, int credit) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "UPDATE users SET credit=? WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {StatusCode::ERR_DB_EXEC, "prepare update credit failed"};
    sqlite3_bind_int(stmt, 1, credit);
    sqlite3_bind_int(stmt, 2, userId);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? StatusOK() : Status{StatusCode::ERR_UNKNOWN, "update credit failed"};
}

Status UserDao::updateStatus(int userId, const string& status) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "UPDATE users SET status=? WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {StatusCode::ERR_DB_EXEC, "prepare update status failed"};
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, userId);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? StatusOK() : Status{StatusCode::ERR_UNKNOWN, "update status failed"};
}
