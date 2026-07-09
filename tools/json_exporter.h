#ifndef JSON_EXPORTER_H
#define JSON_EXPORTER_H

#include "../src/models.h"
#include <QString>
#include <QVector>

class JsonExporter {
public:
    static bool exportBorrowRecords(const QString& filePath, const QVector<BorrowRecord>& records);
    static bool exportBooks(const QString& filePath, const QVector<Book>& books);
    static bool exportStatistics(const QString& filePath, const Statistics& stats);
};

#endif

