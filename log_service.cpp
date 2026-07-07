// log_service.cpp — 审计日志服务
#include "backend.h"
using namespace std;

LogService::LogService(LogDao* dao) : logDao_(dao) {}

// ===== 记录日志 =====
// 输入: adminId(操作的管理员ID), action(操作类型如"delete_book"),
//       target(操作对象描述), detail(详情)
void LogService::log(int adminId, const string& action,
    const string& target, const string& detail)
{
    // TODO: 构造 LogEntry → logDao_->insert()
}

// ===== 查询管理员日志 =====
// 输入: adminId
// 输出: 该管理员的所有操作记录（按时间倒序）
vector<LogEntry> LogService::getAdminLogs(int adminId)
{
    // TODO: logDao_->findByAdmin(adminId)
}

// ===== 查询近期日志 =====
// 输入: days(最近天数)
// 输出: 最近N天的所有操作日志
vector<LogEntry> LogService::getRecentLogs(int days)
{
    // TODO: logDao_->findRecent(days)
}
