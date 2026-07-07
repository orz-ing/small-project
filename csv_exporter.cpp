// csv_exporter.cpp — CSV 导出工具
#include "csv_exporter.h"
using namespace std;

// 输入: 图书列表, 文件路径
// 输出: true=导出成功
bool CsvExporter::exportBooks(const vector<Book>& books, const string& filePath)
{
    // TODO: 表头 "ID,ISBN,书名,作者,出版社,出版日期,分类ID,库存,总库存,位置"
}

// 输入: 用户列表, 文件路径
// 输出: true=导出成功
bool CsvExporter::exportUsers(const vector<User>& users, const string& filePath)
{
    // TODO
}

// 输入: 借阅记录列表, 文件路径
// 输出: true=导出成功
bool CsvExporter::exportBorrowRecords(const vector<BorrowRecord>& records,
    const string& filePath)
{
    // TODO
}

// 输入: 表头列表, 行数据, 文件路径
// 输出: true=导出成功
bool CsvExporter::exportTable(const vector<string>& headers,
    const vector<vector<string>>& rows, const string& filePath)
{
    // TODO: 打开文件 → 写表头 → 逐行写入（逗号分隔） → 关闭
}

// 输入: 原始字符串
// 输出: CSV 转义后字符串（含逗号/引号时自动包裹双引号）
string CsvExporter::escapeCsvField(const string& field)
{
    // TODO: field 含 ',' 或 '"' → 两边加引号，内部引号加倍
}
