#include "backend.h"
#include "utils.h"
AuthService::AuthService(UserDao* dao) : userDao_(dao) {}
User AuthService::login(const string& username, const string& password, Status& status) {
    User user = userDao_->findByUsername(username);
    if (user.getId() == 0) { status = {StatusCode::ERR_USER_NOT_FOUND, "user not found"}; return User(); }
    if (user.getStatus() != "active") { status = {StatusCode::ERR_USER_DISABLED, "user is disabled"}; return User(); }
    string hash = sha256(password);
    if (user.getPasswordHash() != hash) { status = {StatusCode::ERR_WRONG_PASSWORD, "wrong password"}; return User(); }
    status = StatusOK(); return user;
}
User AuthService::registerUser(const string& username, const string& password, const string& realName, const string& role, Status& status) {
    if (userDao_->findByUsername(username).getId() != 0) { status = {StatusCode::ERR_USER_EXISTS, "username already exists"}; return User(); }
    User user(username, sha256(password), realName, role);
    User created = userDao_->insert(user, status);
    return created;
}
