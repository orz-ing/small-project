// utils.cpp — 工具函数
#include "backend.h"
using namespace std;

// ===== 密码哈希 =====
// 输入: 原始字符串
// 输出: SHA-256 十六进制哈希值（64字符）
string sha256(const string& input)
{
    // TODO: 实现 SHA-256
}

// ===== 字符串分割 =====
// 输入: 待分割字符串, 分隔符
// 输出: 分割后的字符串数组
vector<string> split(const string& s, char delim)
{
    // TODO
}

// ===== 日期 → 字符串 =====
// 输入: tm 结构体日期
// 输出: "YYYY-MM-DD" 格式字符串
string dateToString(const tm& date)
{
    // TODO
}

// ===== 字符串 → 日期 =====
// 输入: "YYYY-MM-DD" 格式字符串
// 输出: tm 结构体日期
tm stringToDate(const string& s)
{
    // TODO
}

// ===== 获取今天日期 =====
// 输出: "YYYY-MM-DD" 格式的今天日期
string today()
{
    // TODO
}

// ===== 日期间距 =====
// 输入: 两个 "YYYY-MM-DD" 日期字符串
// 输出: 两者相差的天数（负数表示 d1 < d2）
int daysBetween(const string& d1, const string& d2)
{
    // TODO
}

// ===== 节假日判断 =====
// 输入: "YYYY-MM-DD" 日期
// 输出: true=节假日
bool isHoliday(const string& date)
{
    // TODO
}
