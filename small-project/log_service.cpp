#include "backend.h"
LogService::LogService(LogDao* dao) : logDao_(dao) {}
void LogService::log(int adminId, const string& action, const string& target, const string& detail) {
    LogEntry entry; entry.setAdminId(adminId); entry.setAction(action);
    entry.setTarget(target); entry.setDetail(detail);
    Status s; logDao_->insert(entry, s);
}
vector<LogEntry> LogService::getAdminLogs(int adminId) { return logDao_->findByAdmin(adminId); }
vector<LogEntry> LogService::getRecentLogs(int days) { return logDao_->findRecent(days); }
