#ifndef UTILS_H
#define UTILS_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QVector>
#include <QStringList>
#include <cstdint>

namespace Utils {

// SHA-256 哈希（纯C++实现）
QString sha256(const QString& input);

// 日期工具
QString dateToString(const QDateTime& dt);
QDateTime stringToDate(const QString& str);
QString dateToStringDate(const QDateTime& dt);  // 仅日期部分 yyyy-MM-dd
int daysBetween(const QDateTime& from, const QDateTime& to);
bool isWeekend(const QDateTime& dt);
bool isHoliday(const QDateTime& dt);  // 法定节假日判断

// 字符串工具
QStringList split(const QString& str, const QString& delimiter);
QString join(const QStringList& list, const QString& delimiter);
QString trim(const QString& str);
bool containsIgnoreCase(const QString& str, const QString& keyword);

// JSON工具
template<typename T>
QJsonArray vectorToJsonArray(const QVector<T>& vec) {
    QJsonArray arr;
    for (const auto& item : vec) {
        arr.append(item.toJson());
    }
    return arr;
}

template<typename T>
QVector<T> jsonArrayToVector(const QJsonArray& arr) {
    QVector<T> vec;
    for (const auto& val : arr) {
        vec.append(T::fromJson(val.toObject()));
    }
    return vec;
}

} // namespace Utils

#endif // UTILS_H
