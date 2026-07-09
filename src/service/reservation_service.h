#ifndef RESERVATION_SERVICE_H
#define RESERVATION_SERVICE_H

#include "models.h"
#include "dao/reservation_dao.h"
#include "dao/book_dao.h"
#include "engine/reservation_queue.h"
#include <QSqlDatabase>

class ReservationService {
public:
    explicit ReservationService(QSqlDatabase db);

    Result reserveBook(int userId, int bookId);
    Result cancelReservation(int reservationId);
    QVector<Reservation> getUserReservations(int userId) const;
    QVector<Reservation> getBookReservations(int bookId) const;
    int notifyNextInQueue(int bookId);  // 归还触发，返回被通知的预约ID
    int pendingCount(int bookId) const;

private:
    ReservationDAO m_resDao;
    BookDAO m_bookDao;
    ReservationQueue m_queue;
    QSqlDatabase m_db;
};
#endif
