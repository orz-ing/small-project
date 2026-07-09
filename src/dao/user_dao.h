#ifndef USER_DAO_H
#define USER_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class UserDAO {
public:
    explicit UserDAO(QSqlDatabase db);
    QVector<User> getAll() const;
    User getById(int id) const;
    User getByUsername(const QString& username) const;
    int insert(const User& user);
    bool update(const User& user);
    bool deleteUser(int id);
    bool setDisabled(int userId, bool disabled);
    bool updateCreditScore(int userId, int newScore);
    int count() const;

private:
    User userFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};

#endif
