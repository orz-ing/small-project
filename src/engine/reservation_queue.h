#ifndef RESERVATION_QUEUE_H
#define RESERVATION_QUEUE_H

#include "models.h"
#include <QVector>
#include <QHash>
#include <algorithm>
#include <QDateTime>

// 预约队列：FIFO + 优先级插队 + 过期取消 + 归还触发
class ReservationQueue {
public:
    ReservationQueue() = default;

    // 构建队列（按bookId分组）
    void build(const QVector<Reservation>& reservations);

    // 入队
    void enqueue(const Reservation& reservation);

    // 出队：取队列首位（移除并返回）
    Reservation dequeue(int bookId);

    // 查看队列首位
    Reservation peek(int bookId) const;

    // 取消预约
    bool cancel(int reservationId);

    // 检查并移除过期预约，返回被移除的ID列表
    QVector<int> expireCheck();

    // 获取某本书的队列
    QVector<Reservation> getQueue(int bookId) const;

    // 获取某本书的队列长度
    int queueLength(int bookId) const;

    // 获取用户的全部预约
    QVector<Reservation> getUserReservations(int userId) const;

    // 归还触发：通知队列首位用户有书可借
    // 返回应该被通知的预约ID（队列首位，状态为Pending）
    int notifyNext(int bookId);

    // 清空
    void clear();

private:
    // bookId → 预约队列（按预约时间排序）
    QHash<int, QVector<Reservation>> m_queues;

    // reservationId → bookId 映射
    QHash<int, int> m_reservationMap;
};

#endif
