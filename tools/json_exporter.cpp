#include "json_exporter.h"
#include "../src/utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

bool JsonExporter::exportBorrowRecords(const QString& filePath, const QVector<BorrowRecord>& records) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonDocument doc(Utils::vectorToJsonArray(records));
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool JsonExporter::exportBooks(const QString& filePath, const QVector<Book>& books) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonDocument doc(Utils::vectorToJsonArray(books));
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool JsonExporter::exportStatistics(const QString& filePath, const Statistics& stats) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject root;
    root["totalBooks"] = stats.totalBooks;
    root["totalUsers"] = stats.totalUsers;
    root["activeBorrows"] = stats.activeBorrows;
    root["overdueCount"] = stats.overdueCount;
    root["totalFine"] = stats.totalFine;

    QJsonArray monthlyArr;
    for (const auto& ms : stats.monthlyStats) {
        QJsonObject mo;
        mo["month"] = ms.month;
        mo["borrowCount"] = ms.borrowCount;
        monthlyArr.append(mo);
    }
    root["monthlyStats"] = monthlyArr;

    QJsonArray catArr;
    for (const auto& cs : stats.categoryStats) {
        QJsonObject co;
        co["name"] = cs.categoryName;
        co["bookCount"] = cs.bookCount;
        catArr.append(co);
    }
    root["categoryStats"] = catArr;

    QJsonObject doc;
    doc["statistics"] = root;
    file.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

