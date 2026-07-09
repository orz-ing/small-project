#include "user_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

UserDAO::UserDAO(QSqlDatabase db) : m_db(db) {}

User UserDAO::userFromQuery(QSqlQuery& query) const {
    User u;
    u.id = query.value("id").toInt();
    u.username = query.value("username").toString();
    u.passwordHash = query.value("password_hash").toString();
    u.displayName = query.value("display_name").toString();
    u.role = static_cast<UserRole>(query.value("role").toInt());
    u.creditScore = query.value("credit_score").toInt();
    u.isDisabled = query.value("is_disabled").toBool();
    u.email = query.value("email").toString();
    u.phone = query.value("phone").toString();
    u.createTime = QDateTime::fromString(query.value("create_time").toString(), Qt::ISODate);
    return u;
}

QVector<User> UserDAO::getAll() const {
    QVector<User> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM users ORDER BY id");
    while (query.next()) result.append(userFromQuery(query));
    return result;
}

User UserDAO::getById(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM users WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) return userFromQuery(query);
    return User{};
}

User UserDAO::getByUsername(const QString& username) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) return userFromQuery(query);
    return User{};
}

int UserDAO::insert(const User& user) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (username, password_hash, display_name, role, "
        "credit_score, email, phone) VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(user.username);
    query.addBindValue(user.passwordHash);
    query.addBindValue(user.displayName);
    query.addBindValue(static_cast<int>(user.role));
    query.addBindValue(user.creditScore);
    query.addBindValue(user.email);
    query.addBindValue(user.phone);
    if (query.exec()) return query.lastInsertId().toInt();
    qWarning() << "Insert user failed:" << query.lastError().text();
    return -1;
}

bool UserDAO::update(const User& user) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE users SET display_name=?, role=?, email=?, phone=? WHERE id=?"
    );
    query.addBindValue(user.displayName);
    query.addBindValue(static_cast<int>(user.role));
    query.addBindValue(user.email);
    query.addBindValue(user.phone);
    query.addBindValue(user.id);
    return query.exec();
}

bool UserDAO::deleteUser(int id) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool UserDAO::setDisabled(int userId, bool disabled) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET is_disabled = ? WHERE id = ?");
    query.addBindValue(disabled ? 1 : 0);
    query.addBindValue(userId);
    return query.exec();
}

bool UserDAO::updateCreditScore(int userId, int newScore) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET credit_score = ? WHERE id = ?");
    query.addBindValue(newScore);
    query.addBindValue(userId);
    return query.exec();
}

int UserDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
