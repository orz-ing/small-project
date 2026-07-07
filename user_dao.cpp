// user_dao.cpp — 用户数据访问对象
#include "backend.h"
using namespace std;

UserDao::UserDao(DatabaseManager& db) : db_(db) {}

// ===== 按用户名查找 =====
// 输入: username
// 输出: User 对象; id=0 表示不存在
User UserDao::findByUsername(const string& username)
{
    // TODO: SELECT * FROM users WHERE username=? → rowToUser()
}

// ===== 按ID查找 =====
// 输入: userId
// 输出: User 对象
User UserDao::findById(int userId)
{
    // TODO
}

// ===== 查找全部 =====
// 输出: 所有用户列表
vector<User> UserDao::findAll()
{
    // TODO: SELECT * FROM users
}

// ===== 插入 =====
// 输入: User 对象（id 字段忽略，自动生成）
// 输出: 新插入记录的 id; -1=失败
int UserDao::insert(const User& user)
{
    // TODO: INSERT INTO users(...) VALUES(...)
}

// ===== 更新 =====
// 输入: User 对象（按 id 匹配更新除 credit/status 外字段）
// 输出: true=成功
bool UserDao::update(const User& user)
{
    // TODO: UPDATE users SET ... WHERE id=?
}

// ===== 更新信用分 =====
// 输入: userId, 新信用分值
// 输出: true=成功
bool UserDao::updateCredit(int userId, int credit)
{
    // TODO: UPDATE users SET credit=? WHERE id=?
}

// ===== 更新状态 =====
// 输入: userId, 状态("active"/"disabled")
// 输出: true=成功
bool UserDao::updateStatus(int userId, const string& status)
{
    // TODO: UPDATE users SET status=? WHERE id=?
}

// ===== 行映射（private）=====
// 输入: sqlite3_stmt*
// 输出: User 对象（从当前行提取字段）
User UserDao::rowToUser(void* stmt) const
{
    // TODO: sqlite3_column_xxx(stmt, 0), sqlite3_column_xxx(stmt, 1) ...
}
