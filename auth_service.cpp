// auth_service.cpp — 认证服务
#include "backend.h"
using namespace std;

// ===== 构造函数 =====
// 输入: UserDao 指针
AuthService::AuthService(UserDao* dao) : userDao_(dao) {}

// ===== 登录 =====
// 输入: username(账号), password(明文密码)
// 输出: User 对象; id=0 表示登录失败（用户不存在或密码错误）
User AuthService::login(const string& username, const string& password)
{
    // TODO: sha256(password) → userDao_->findByUsername() → 比对密码哈希
}

// ===== 注册 =====
// 输入: username(账号), password(明文), realName, role("reader"/"admin")
// 输出: true=注册成功; false=用户名已存在
bool AuthService::registerUser(const string& username, const string& password,
    const string& realName, const string& role)
{
    // TODO: 检查用户名唯一 → sha256 → userDao_->insert()
}
