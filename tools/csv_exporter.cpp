#include "csv_exporter.h"
#include <QFile>
#include <QTextStream>

bool CsvExporter::exportBorrowRecords(const QString& filePath, const QVector<BorrowRecord>& records) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "ID,用户ID,图书ID,书名,ISBN,借阅日期,应还日期,归还日期,罚款,状态\n";

    for (const auto& r : records) {
        auto status = r.status == BorrowStatus::Returned ? "已归还" :
                      r.status == BorrowStatus::Overdue ? "逾期" :
                      r.status == BorrowStatus::Lost ? "丢失" : "借阅中";
        out << r.id << ","
            << r.userId << ","
            << r.bookId << ","
            << "\"" << r.bookTitle << "\","
            << r.bookIsbn << ","
            << r.borrowDate.toString("yyyy-MM-dd") << ","
            << r.dueDate.toString("yyyy-MM-dd") << ","
            << (r.returnDate.isValid() ? r.returnDate.toString("yyyy-MM-dd") : "") << ","
            << QString::number(r.fine, 'f', 2) << ","
            << status << "\n";
    }

    file.close();
    return true;
}

bool CsvExporter::exportBooks(const QString& filePath, const QVector<Book>& books) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "ID,书名,作者,ISBN,出版社,分类,总库存,可用库存\n";

    for (const auto& b : books) {
        out << b.id << ","
            << "\"" << b.title << "\","
            << "\"" << b.author << "\","
            << b.isbn << ","
            << "\"" << b.publisher << "\","
            << b.categoryPath << ","
            << b.totalStock << ","
            << b.availableStock << "\n";
    }

    file.close();
    return true;
}

bool CsvExporter::exportUsers(const QString& filePath, const QVector<User>& users) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "ID,用户名,显示名,角色,信用分,状态\n";

    for (const auto& u : users) {
        out << u.id << ","
            << u.username << ","
            << u.displayName << ","
            << (u.role == UserRole::Admin ? "管理员" : "读者") << ","
            << u.creditScore << ","
            << (u.isDisabled ? "已禁用" : "正常") << "\n";
    }

    file.close();
    return true;
}

