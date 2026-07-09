#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include "models.h"
#include "dao/log_dao.h"
#include <QSqlDatabase>

class LogService {
public:
    explicit LogService(QSqlDatabase db);

    void log(int adminId, const QString& adminName, const QString& action,
             const QString& targetType, int targetId, const QString& detail);
    QVector<LogEntry> getRecentLogs() const;
    QVector<LogEntry> getLogsByAction(const QString& action) const;

private:
    LogDAO m_logDao;
};

#endif
