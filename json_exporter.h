// json_exporter.h — JSON 导出工具
#ifndef JSON_EXPORTER_H
#define JSON_EXPORTER_H

#include "backend.h"

class JsonExporter {
public:
    // 输入: 图书列表, 输出文件路径
    // 输出: true=导出成功, JSON 格式: [{"id":1,"title":"...","author":"..."}]
    static bool exportBooks(const vector<Book>& books, const string& filePath);

    // 输入: 用户列表, 输出文件路径
    // 输出: true=导出成功
    static bool exportUsers(const vector<User>& users, const string& filePath);

    // 输入: 借阅记录列表, 输出文件路径
    // 输出: true=导出成功
    static bool exportBorrowRecords(const vector<BorrowRecord>& records,
                                    const string& filePath);

    // 输入: 任意数据(map 列表), 输出文件路径
    // 输出: true=导出成功
    static bool exportJson(const vector<map<string,string>>& data,
                           const string& filePath);

private:
    // 输入: 数据; 输出: JSON 序列化后的字符串
    static string serialize(const vector<map<string,string>>& data);
};

#endif
