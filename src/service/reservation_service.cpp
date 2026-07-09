#include "reservation_service.h"
#include <QSqlQuery>
#include <QSqlError>

ReservationService::ReservationService(QSqlDatabase db)
    : m_resDao(db), m_bookDao(db), m_db(db) {
    // 初始化时加载已有预约到队列
    m_queue.build(m_resDao.getAll());
}

Result ReservationService::reserveBook(int userId, int bookId) {
    Book book = m_bookDao.getById(bookId);
    if (book.id == 0) return Result::fail("图书不存在");

    // 检查是否已有预约
    QVector<Reservation> existing = m_resDao.getByUser(userId);
    for (const auto& r : existing) {
        if (r.bookId == bookId && r.status == ReservationStatus::Pending) {
            return Result::fail("您已预约过这本书");
        }
    }

    Reservation res;
    res.userId = userId;
    res.bookId = bookId;
    res.bookTitle = book.title;
    res.reserveDate = QDateTime::currentDateTime();
    res.expireDate = res.reserveDate.addSecs(48 * 3600); // 48小时过期
    res.status = ReservationStatus::Pending;

    int id = m_resDao.insert(res);
    if (id <= 0) return Result::fail("预约失败");

    // 加入队列
    res.id = id;
    m_queue.enqueue(res);

    return Result::ok("预约成功，排队位置 #" + QString::number(m_queue.queueLength(bookId)));
}

Result ReservationService::cancelReservation(int reservationId) {
    if (m_queue.cancel(reservationId)) {
        if (m_resDao.cancelReservation(reservationId)) {
            return Result::ok("已取消预约");
        }
    }
    return Result::fail("取消预约失败，或预约不存在");
}

QVector<Reservation> ReservationService::getUserReservations(int userId) const {
    return m_resDao.getByUser(userId);
}

QVector<Reservation> ReservationService::getBookReservations(int bookId) const {
    return m_resDao.getByBook(bookId);
}

int ReservationService::notifyNextInQueue(int bookId) {
    return m_queue.notifyNext(bookId);
}

int ReservationService::pendingCount(int bookId) const {
    return m_queue.queueLength(bookId);
}
