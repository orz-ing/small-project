// csv_exporter.h — CSV 导出工具
#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "backend.h"

class CsvExporter {
public:
    // 输入: 图书列表, 输出文件路径
    // 输出: true=导出成功, false=失败（权限/路径无效）
    static bool exportBooks(const vector<Book>& books, const string& filePath);

    // 输入: 用户列表, 输出文件路径
    // 输出: true=导出成功
    static bool exportUsers(const vector<User>& users, const string& filePath);

    // 输入: 借阅记录列表, 输出文件路径
    // 输出: true=导出成功
    static bool exportBorrowRecords(const vector<BorrowRecord>& records,
                                    const string& filePath);

    // 输入: 任意二维数据(表头+行数据), 输出文件路径
    // 输出: true=导出成功
    static bool exportTable(const vector<string>& headers,
                            const vector<vector<string>>& rows,
                            const string& filePath);

private:
    // 输入: 字符串; 输出: CSV 转义后的字符串（含逗号/引号时自动加引号）
    static string escapeCsvField(const string& field);
};

#endif
