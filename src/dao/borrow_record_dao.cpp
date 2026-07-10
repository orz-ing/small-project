#include "borrow_record_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

BorrowRecordDAO::BorrowRecordDAO(QSqlDatabase db) : m_db(db) {}

BorrowRecord BorrowRecordDAO::recordFromQuery(QSqlQuery& query) const {
    BorrowRecord r;
    r.id = query.value("id").toInt();
    r.userId = query.value("user_id").toInt();
    r.bookId = query.value("book_id").toInt();
    r.bookTitle = query.value("book_title").toString();
    r.bookIsbn = query.value("book_isbn").toString();
    r.borrowDate = QDateTime::fromString(query.value("borrow_date").toString(), Qt::ISODate);
    r.dueDate = QDateTime::fromString(query.value("due_date").toString(), Qt::ISODate);
    QString rd = query.value("return_date").toString();
    if (!rd.isEmpty()) r.returnDate = QDateTime::fromString(rd, Qt::ISODate);
    r.fine = query.value("fine").toDouble();
    r.status = static_cast<BorrowStatus>(query.value("status").toInt());
    return r;
}

QVector<BorrowRecord> BorrowRecordDAO::getAll() const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM borrow_records ORDER BY id DESC");
    while (query.next()) result.append(recordFromQuery(query));
    return result;
}

QVector<BorrowRecord> BorrowRecordDAO::getByUser(int userId) const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM borrow_records WHERE user_id = ? ORDER BY borrow_date DESC");
    query.addBindValue(userId);
    if (query.exec()) {
        while (query.next()) result.append(recordFromQuery(query));
    }
    return result;
}

QVector<BorrowRecord> BorrowRecordDAO::getByBook(int bookId) const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM borrow_records WHERE book_id = ? ORDER BY borrow_date DESC");
    query.addBindValue(bookId);
    if (query.exec()) {
        while (query.next()) result.append(recordFromQuery(query));
    }
    return result;
}

QVector<BorrowRecord> BorrowRecordDAO::getActiveByUser(int userId) const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM borrow_records WHERE user_id = ? AND status = 0 ORDER BY due_date");
    query.addBindValue(userId);
    if (query.exec()) {
        while (query.next()) result.append(recordFromQuery(query));
    }
    return result;
}

QVector<BorrowRecord> BorrowRecordDAO::getOverdue() const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM borrow_records WHERE status = 0 AND due_date < datetime('now','localtime')");
    while (query.next()) result.append(recordFromQuery(query));
    return result;
}

QVector<BorrowRecord> BorrowRecordDAO::getByDate(const QDateTime& start, const QDateTime& end) const {
    QVector<BorrowRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM borrow_records WHERE borrow_date >= ? AND borrow_date <= ? ORDER BY borrow_date");
    query.addBindValue(start.toString(Qt::ISODate));
    query.addBindValue(end.toString(Qt::ISODate));
    if (query.exec()) {
        while (query.next()) result.append(recordFromQuery(query));
    }
    return result;
}

BorrowRecord BorrowRecordDAO::getById(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM borrow_records WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) return recordFromQuery(query);
    return BorrowRecord{};
}

int BorrowRecordDAO::insert(const BorrowRecord& record) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO borrow_records (user_id, book_id, book_title, book_isbn, "
          "borrow_date, due_date, return_date, fine, status) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(record.userId);
    query.addBindValue(record.bookId);
    query.addBindValue(record.bookTitle);
    query.addBindValue(record.bookIsbn);
    query.addBindValue(record.borrowDate.toString(Qt::ISODate));
    query.addBindValue(record.dueDate.toString(Qt::ISODate));
    query.addBindValue(record.returnDate.isValid() ? record.returnDate.toString(Qt::ISODate) : QVariant());
    query.addBindValue(record.fine);
    query.addBindValue(static_cast<int>(BorrowStatus::Borrowing));
    if (query.exec()) return query.lastInsertId().toInt();
    qWarning() << "Insert borrow record failed:" << query.lastError().text();
    return -1;
}

bool BorrowRecordDAO::update(const BorrowRecord& record) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE borrow_records SET book_title=?, book_isbn=?, status=?, fine=? WHERE id=?"
    );
    query.addBindValue(record.bookTitle);
    query.addBindValue(record.bookIsbn);
    query.addBindValue(static_cast<int>(record.status));
    query.addBindValue(record.fine);
    query.addBindValue(record.id);
    return query.exec();
}

bool BorrowRecordDAO::returnBook(int recordId, const QDateTime& returnDate, double fine) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE borrow_records SET return_date=?, fine=?, status=? WHERE id=?"
    );
    query.addBindValue(returnDate.toString(Qt::ISODate));
    query.addBindValue(fine);
    query.addBindValue(static_cast<int>(BorrowStatus::Returned));
    query.addBindValue(recordId);
    return query.exec();
}

int BorrowRecordDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM borrow_records");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

int BorrowRecordDAO::activeCount() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE status = 0");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

int BorrowRecordDAO::overdueCount() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM borrow_records WHERE status = 0 AND due_date < datetime('now','localtime')");
    if (query.next()) return query.value(0).toInt();
    return 0;
}