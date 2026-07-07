// log_dao.cpp — 日志数据访问对象
#include "backend.h"
using namespace std;

LogDao::LogDao(DatabaseManager& db) : db_(db) {}

// 输入: LogEntry 对象
// 输出: 新插入记录的 id
int LogDao::insert(const LogEntry& log)
{
    // TODO: INSERT INTO log_entries
}

// 输入: adminId
// 输出: 该管理员的所有操作日志（按时间倒序）
vector<LogEntry> LogDao::findByAdmin(int adminId)
{
    // TODO: SELECT * FROM log_entries WHERE admin_id=? ORDER BY created_at DESC
}

// 输入: days(最近天数)
// 输出: 最近N天内所有操作日志
vector<LogEntry> LogDao::findRecent(int days)
{
    // TODO: SELECT * FROM log_entries WHERE created_at >= datetime('now', ?)
}
