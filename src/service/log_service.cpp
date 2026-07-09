#include "log_service.h"
#include <QDateTime>

LogService::LogService(QSqlDatabase db) : m_logDao(db) {}

void LogService::log(int adminId, const QString& adminName, const QString& action,
                      const QString& targetType, int targetId, const QString& detail) {
    LogEntry entry;
    entry.adminId = adminId;
    entry.adminName = adminName;
    entry.action = action;
    entry.targetType = targetType;
    entry.targetId = targetId;
    entry.detail = detail;
    entry.timestamp = QDateTime::currentDateTime();
    m_logDao.insert(entry);
}

QVector<LogEntry> LogService::getRecentLogs() const {
    return m_logDao.getAll();
}

QVector<LogEntry> LogService::getLogsByAction(const QString& action) const {
    return m_logDao.getByAction(action);
}
