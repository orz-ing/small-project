// reservation_dao.cpp — 预约记录数据访问对象
#include "backend.h"
using namespace std;

ReservationDao::ReservationDao(DatabaseManager& db) : db_(db) {}

int ReservationDao::insert(const Reservation& r)
{
    // TODO: INSERT INTO reservations
}

vector<Reservation> ReservationDao::findByUser(int userId)
{
    // TODO: SELECT * FROM reservations WHERE user_id=? ORDER BY reserve_date
}

// 输入: bookId
// 输出: 该书的预约列表（按预约时间升序排列）
vector<Reservation> ReservationDao::findByBook(int bookId)
{
    // TODO: SELECT * FROM reservations WHERE book_id=? AND status='pending' ORDER BY reserve_date
}

// 输入: reservationId, status("pending"/"fulfilled"/"cancelled"/"expired")
// 输出: true=更新成功
bool ReservationDao::updateStatus(int reservationId, const string& status)
{
    // TODO: UPDATE reservations SET status=? WHERE id=?
}

// 输入: timeoutHours(48)
// 输出: 超时未确认的预约列表
vector<Reservation> ReservationDao::findExpired(int timeoutHours)
{
    // TODO: SELECT * FROM reservations WHERE status='pending' AND reserve_date < datetime('now', ?)
}

Reservation ReservationDao::rowToReservation(void* stmt) const
{
    // TODO: sqlite3 行映射
}
