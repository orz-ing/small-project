#ifndef BORROW_RECORD_DAO_H
#define BORROW_RECORD_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class BorrowRecordDAO {
public:
    explicit BorrowRecordDAO(QSqlDatabase db);
    QVector<BorrowRecord> getAll() const;
    QVector<BorrowRecord> getByUser(int userId) const;
    QVector<BorrowRecord> getByBook(int bookId) const;
    QVector<BorrowRecord> getActiveByUser(int userId) const;
    QVector<BorrowRecord> getOverdue() const;
    QVector<BorrowRecord> getByDate(const QDateTime& start, const QDateTime& end) const;
    BorrowRecord getById(int id) const;
    int insert(const BorrowRecord& record);
    bool update(const BorrowRecord& record);
    bool returnBook(int recordId, const QDateTime& returnDate, double fine);
    int count() const;
    int activeCount() const;
    int overdueCount() const;

private:
    BorrowRecord recordFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};

#endif
