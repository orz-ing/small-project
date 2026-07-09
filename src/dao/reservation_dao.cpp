#include "reservation_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

ReservationDAO::ReservationDAO(QSqlDatabase db) : m_db(db) {}

Reservation ReservationDAO::resFromQuery(QSqlQuery& query) const {
    Reservation r;
    r.id = query.value("id").toInt();
    r.userId = query.value("user_id").toInt();
    r.bookId = query.value("book_id").toInt();
    r.bookTitle = query.value("book_title").toString();
    r.reserveDate = QDateTime::fromString(query.value("reserve_date").toString(), Qt::ISODate);
    r.expireDate = QDateTime::fromString(query.value("expire_date").toString(), Qt::ISODate);
    r.priority = query.value("priority").toInt();
    r.status = static_cast<ReservationStatus>(query.value("status").toInt());
    return r;
}

QVector<Reservation> ReservationDAO::getAll() const {
    QVector<Reservation> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM reservations ORDER BY id DESC");
    while (query.next()) result.append(resFromQuery(query));
    return result;
}

Reservation ReservationDAO::getById(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM reservations WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) return resFromQuery(query);
    return Reservation{};
}

QVector<Reservation> ReservationDAO::getByUser(int userId) const {
    QVector<Reservation> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM reservations WHERE user_id = ? ORDER BY reserve_date DESC");
    query.addBindValue(userId);
    if (query.exec()) { while (query.next()) result.append(resFromQuery(query)); }
    return result;
}

QVector<Reservation> ReservationDAO::getByBook(int bookId) const {
    QVector<Reservation> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM reservations WHERE book_id = ? ORDER BY reserve_date");
    query.addBindValue(bookId);
    if (query.exec()) { while (query.next()) result.append(resFromQuery(query)); }
    return result;
}

QVector<Reservation> ReservationDAO::getPendingByBook(int bookId) const {
    QVector<Reservation> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM reservations WHERE book_id = ? AND status = 0 ORDER BY reserve_date");
    query.addBindValue(bookId);
    if (query.exec()) { while (query.next()) result.append(resFromQuery(query)); }
    return result;
}

int ReservationDAO::insert(const Reservation& res) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO reservations (user_id, book_id, book_title, reserve_date, expire_date, priority, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    query.addBindValue(res.userId);
    query.addBindValue(res.bookId);
    query.addBindValue(res.bookTitle);
    query.addBindValue(res.reserveDate.toString(Qt::ISODate));
    query.addBindValue(res.expireDate.toString(Qt::ISODate));
    query.addBindValue(res.priority);
    query.addBindValue(static_cast<int>(ReservationStatus::Pending));
    if (query.exec()) return query.lastInsertId().toInt();
    qWarning() << "Insert reservation failed:" << query.lastError().text();
    return -1;
}

bool ReservationDAO::updateStatus(int reservationId, ReservationStatus status) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE reservations SET status = ? WHERE id = ?");
    query.addBindValue(static_cast<int>(status));
    query.addBindValue(reservationId);
    return query.exec();
}

bool ReservationDAO::cancelReservation(int reservationId) {
    return updateStatus(reservationId, ReservationStatus::Cancelled);
}

int ReservationDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM reservations");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
