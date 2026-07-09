#ifndef RESERVATION_DAO_H
#define RESERVATION_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class ReservationDAO {
public:
    explicit ReservationDAO(QSqlDatabase db);
    QVector<Reservation> getAll() const;
    Reservation getById(int id) const;
    QVector<Reservation> getByUser(int userId) const;
    QVector<Reservation> getByBook(int bookId) const;
    QVector<Reservation> getPendingByBook(int bookId) const;
    int insert(const Reservation& res);
    bool updateStatus(int reservationId, ReservationStatus status);
    bool cancelReservation(int reservationId);
    int count() const;
private:
    Reservation resFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};
#endif
