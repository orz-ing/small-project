// json_exporter.cpp — JSON 导出工具
#include "json_exporter.h"
using namespace std;

// 输入: 图书列表, 文件路径
// 输出: true=导出成功
bool JsonExporter::exportBooks(const vector<Book>& books, const string& filePath)
{
    // TODO: Book → map<string,string> → serialize → 写文件
}

// 输入: 用户列表, 文件路径
// 输出: true=导出成功
bool JsonExporter::exportUsers(const vector<User>& users, const string& filePath)
{
    // TODO
}

// 输入: 借阅记录列表, 文件路径
// 输出: true=导出成功
bool JsonExporter::exportBorrowRecords(const vector<BorrowRecord>& records,
    const string& filePath)
{
    // TODO
}

// 输入: map 列表(泛用数据), 文件路径
// 输出: true=导出成功
bool JsonExporter::exportJson(const vector<map<string,string>>& data,
    const string& filePath)
{
    // TODO: serialize → 写文件
}

// 输入: map 列表
// 输出: JSON 格式字符串
string JsonExporter::serialize(const vector<map<string,string>>& data)
{
    // TODO: 手动拼接 JSON（不依赖第三方库）
}
