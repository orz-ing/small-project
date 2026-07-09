#include "reservation_queue.h"

void ReservationQueue::build(const QVector<Reservation>& reservations) {
    clear();
    for (const auto& res : reservations) {
        if (res.status == ReservationStatus::Pending) {
            m_queues[res.bookId].append(res);
            m_reservationMap[res.id] = res.bookId;
        }
    }
    // 每本书的队列按预约时间排序（FIFO）
    for (auto it = m_queues.begin(); it != m_queues.end(); ++it) {
        std::sort(it->begin(), it->end(),
            [](const Reservation& a, const Reservation& b) {
                if (a.priority != b.priority)
                    return a.priority > b.priority; // 高优先级在前
                return a.reserveDate < b.reserveDate; // 先预约在前
            });
    }
}

void ReservationQueue::enqueue(const Reservation& reservation) {
    m_queues[reservation.bookId].append(reservation);
    m_reservationMap[reservation.id] = reservation.bookId;
    // 重新排序
    auto& q = m_queues[reservation.bookId];
    std::sort(q.begin(), q.end(),
        [](const Reservation& a, const Reservation& b) {
            if (a.priority != b.priority)
                return a.priority > b.priority;
            return a.reserveDate < b.reserveDate;
        });
}

Reservation ReservationQueue::dequeue(int bookId) {
    auto it = m_queues.find(bookId);
    if (it == m_queues.end() || it->isEmpty()) return Reservation{};

    Reservation front = it->first();
    it->removeFirst();
    m_reservationMap.remove(front.id);
    return front;
}

Reservation ReservationQueue::peek(int bookId) const {
    auto it = m_queues.find(bookId);
    if (it == m_queues.end() || it->isEmpty()) return Reservation{};
    return it->first();
}

bool ReservationQueue::cancel(int reservationId) {
    auto mapIt = m_reservationMap.find(reservationId);
    if (mapIt == m_reservationMap.end()) return false;

    int bookId = mapIt.value();
    auto& q = m_queues[bookId];
    for (int i = 0; i < q.size(); ++i) {
        if (q[i].id == reservationId) {
            q.removeAt(i);
            m_reservationMap.remove(reservationId);
            return true;
        }
    }
    return false;
}

QVector<int> ReservationQueue::expireCheck() {
    QVector<int> expiredIds;
    QDateTime now = QDateTime::currentDateTime();

    for (auto it = m_queues.begin(); it != m_queues.end(); ) {
        auto& q = it.value();
        QVector<Reservation> remaining;
        for (const auto& res : q) {
            if (res.expireDate.isValid() && now > res.expireDate) {
                expiredIds.append(res.id);
                m_reservationMap.remove(res.id);
            } else {
                remaining.append(res);
            }
        }
        if (remaining.isEmpty()) {
            it = m_queues.erase(it);
        } else {
            q = remaining;
            ++it;
        }
    }
    return expiredIds;
}

QVector<Reservation> ReservationQueue::getQueue(int bookId) const {
    auto it = m_queues.find(bookId);
    if (it == m_queues.end()) return {};
    return it.value();
}

int ReservationQueue::queueLength(int bookId) const {
    auto it = m_queues.find(bookId);
    return (it == m_queues.end()) ? 0 : it.value().size();
}

QVector<Reservation> ReservationQueue::getUserReservations(int userId) const {
    QVector<Reservation> result;
    for (const auto& q : m_queues) {
        for (const auto& res : q) {
            if (res.userId == userId) {
                result.append(res);
            }
        }
    }
    return result;
}

int ReservationQueue::notifyNext(int bookId) {
    auto front = peek(bookId);
    if (front.id == 0) return 0;
    return front.id; // 返回应该被通知的预约ID
}

void ReservationQueue::clear() {
    m_queues.clear();
    m_reservationMap.clear();
}
