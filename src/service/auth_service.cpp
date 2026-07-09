#include "auth_service.h"
#include "utils.h"
#include "engine/credit_engine.h"

AuthService::AuthService(QSqlDatabase db) : m_userDao(db) {}

Result AuthService::registerUser(const QString& username, const QString& password,
                                  const QString& displayName, UserRole role) {
    if (username.trimmed().isEmpty()) {
        return Result::fail("用户名不能为空");
    }
    if (password.length() < 6) {
        return Result::fail("密码长度不能少于6位");
    }

    // 检查用户名是否已存在
    User existing = m_userDao.getByUsername(username);
    if (existing.id > 0) {
        return Result::fail("用户名已被注册");
    }

    User user;
    user.username = username;
    user.passwordHash = Utils::sha256(password);
    user.displayName = displayName.isEmpty() ? username : displayName;
    user.role = role;
    user.creditScore = CreditEngine::INITIAL_SCORE;

    int id = m_userDao.insert(user);
    if (id > 0) {
        return Result::ok("注册成功");
    }
    return Result::fail("注册失败，请稍后重试");
}

Result AuthService::login(const QString& username, const QString& password) {
    if (username.trimmed().isEmpty()) {
        return Result::fail("请输入用户名");
    }

    User user = m_userDao.getByUsername(username);
    if (user.id == 0) {
        return Result::fail("用户名或密码错误");
    }

    if (user.isDisabled) {
        return Result::fail("账号已被禁用，请联系管理员");
    }

    QString hash = Utils::sha256(password);
    if (user.passwordHash != hash) {
        return Result::fail("用户名或密码错误");
    }

    m_currentUser = user;
    return Result::ok("登录成功", QVariant::fromValue(user.toJson()));
}

void AuthService::logout() {
    m_currentUser = User{};
}
