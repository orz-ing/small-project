#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "../src/models.h"
#include <QString>
#include <QVector>

class CsvExporter {
public:
    static bool exportBorrowRecords(const QString& filePath, const QVector<BorrowRecord>& records);
    static bool exportBooks(const QString& filePath, const QVector<Book>& books);
    static bool exportUsers(const QString& filePath, const QVector<User>& users);
};

#endif

