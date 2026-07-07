// reservation_queue.cpp — FIFO 预约队列（线程安全）
#include "backend.h"
using namespace std;

// ===== 入队 =====
// 输入: bookId(图书ID), userId(用户ID), priority(0=普通,1=优先)
// 输出: 排队位置（从1开始）; -1=已存在该用户的预约
int ReservationQueue::enqueue(int bookId, int userId, int priority)
{
    // TODO: 加锁 → 检查重复 → 插入队列 → 返回位置
}

// ===== 出队（归还触发）=====
// 输入: bookId
// 输出: 队首 userId; -1=队列为空
int ReservationQueue::dequeue(int bookId)
{
    // TODO: 加锁 → 弹出队首 → 返回 userId
}

// ===== 取消预约 =====
// 输入: bookId, userId
void ReservationQueue::cancel(int bookId, int userId)
{
    // TODO: 加锁 → 从队列中删除该用户
}

// ===== 查询排队位置 =====
// 输入: bookId, userId
// 输出: 排队位置（从1开始）; -1=不在队列中
int ReservationQueue::getPosition(int bookId, int userId) const
{
    // TODO: 只读锁 → 遍历队列查找
}

// ===== 超时处理 =====
// 输入: timeoutHours(超时小时数, 默认48)
// 输出: (bookId, userId) 已超时的预约列表
vector<pair<int,int>> ReservationQueue::processExpired(int timeoutHours)
{
    // TODO: 加锁 → 遍历所有队列 → 移除超时项 → 返回超时列表
}

// ===== 队列长度 =====
// 输入: bookId
// 输出: 该书的预约队列长度
int ReservationQueue::size(int bookId) const
{
    // TODO
}

// ===== 查看队首（不弹出）=====
// 输入: bookId
// 输出: 队首 userId; -1=队列空
int ReservationQueue::peekFront(int bookId) const
{
    // TODO
}
