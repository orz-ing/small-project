#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "models.h"
#include "dao/user_dao.h"
#include <QSqlDatabase>

class AuthService {
public:
    explicit AuthService(QSqlDatabase db);

    // 注册
    Result registerUser(const QString& username, const QString& password,
                        const QString& displayName, UserRole role = UserRole::Reader);

    // 登录
    Result login(const QString& username, const QString& password);

    // 登出
    void logout();

    // 获取当前用户
    User currentUser() const { return m_currentUser; }
    bool isLoggedIn() const { return m_currentUser.id > 0; }
    bool isAdmin() const { return isLoggedIn() && m_currentUser.role == UserRole::Admin; }

private:
    UserDAO m_userDao;
    User m_currentUser;
};

#endif
