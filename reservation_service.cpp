// reservation_service.cpp — 预约服务
#include "backend.h"
using namespace std;

ReservationService::ReservationService(ReservationDao* dao, ReservationQueue* q)
    : reserveDao_(dao), queue_(q) {}

// ===== 预约 =====
// 输入: userId, bookId
// 输出: 排队位置（从1开始）; -1=预约失败（已有预约/可借库存>0）
int ReservationService::reserveBook(int userId, int bookId)
{
    // TODO: 检查库存 → stock>0 直接走借阅 → 否则 enqueue → insert DB
}

// ===== 取消预约 =====
// 输入: reservationId
// 输出: true=取消成功
bool ReservationService::cancelReservation(int reservationId)
{
    // TODO: reserveDao_->updateStatus(resId, "cancelled") → queue_->cancel()
}

// ===== 我的预约 =====
// 输入: userId
// 输出: 预约记录列表
vector<Reservation> ReservationService::getMyReservations(int userId)
{
    // TODO: reserveDao_->findByUser(userId)
}

// ===== 归还触发 =====
// 输入: 刚归还的 bookId
void ReservationService::processReturn(int bookId)
{
    // TODO: queue_->dequeue(bookId) → 通知队首用户 → 更新预约状态 "fulfilled"
}

// ===== 超时处理 =====
void ReservationService::processExpired()
{
    // TODO: queue_->processExpired(48) → reserveDao_->updateStatus(..., "expired")
}
