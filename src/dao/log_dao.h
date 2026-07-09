#ifndef LOG_DAO_H
#define LOG_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class LogDAO {
public:
    explicit LogDAO(QSqlDatabase db);
    QVector<LogEntry> getAll() const;
    QVector<LogEntry> getByAction(const QString& action) const;
    int insert(const LogEntry& entry);
    int count() const;
private:
    LogEntry entryFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};
#endif
