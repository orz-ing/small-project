#include "log_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

LogDAO::LogDAO(QSqlDatabase db) : m_db(db) {}

LogEntry LogDAO::entryFromQuery(QSqlQuery& query) const {
    LogEntry e;
    e.id = query.value("id").toInt();
    e.adminId = query.value("admin_id").toInt();
    e.adminName = query.value("admin_name").toString();
    e.action = query.value("action").toString();
    e.targetType = query.value("target_type").toString();
    e.targetId = query.value("target_id").toInt();
    e.detail = query.value("detail").toString();
    e.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);
    return e;
}

QVector<LogEntry> LogDAO::getAll() const {
    QVector<LogEntry> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM logs ORDER BY id DESC LIMIT 100");
    while (query.next()) result.append(entryFromQuery(query));
    return result;
}

QVector<LogEntry> LogDAO::getByAction(const QString& action) const {
    QVector<LogEntry> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM logs WHERE action = ? ORDER BY id DESC LIMIT 100");
    query.addBindValue(action);
    if (query.exec()) { while (query.next()) result.append(entryFromQuery(query)); }
    return result;
}

int LogDAO::insert(const LogEntry& entry) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO logs (admin_id, admin_name, action, target_type, target_id, detail) "
        "VALUES (?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(entry.adminId);
    query.addBindValue(entry.adminName);
    query.addBindValue(entry.action);
    query.addBindValue(entry.targetType);
    query.addBindValue(entry.targetId);
    query.addBindValue(entry.detail);
    if (query.exec()) return query.lastInsertId().toInt();
    return -1;
}

int LogDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM logs");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
