#include "backend.h"
#include "utils.h"

ReservationService::ReservationService(ReservationDao* dao, ReservationQueue* q)
    : reserveDao_(dao), queue_(q) {}

Reservation ReservationService::reserveBook(int userId, int bookId, Status& status) {
    auto existing = reserveDao_->findByUser(userId);
    for (auto& r : existing) {
        if (r.getBookId() == bookId && r.getStatus() == "pending") {
            status = {StatusCode::ERR_RESERVATION_EXISTS, "already reserved"};
            return Reservation();
        }
    }
    Reservation r;
    r.setUserId(userId); r.setBookId(bookId);
    r.setReserveDate(today()); r.setExpireDate(addDays(today(), 2));
    r.setStatus("pending"); r.setPriority(0);
    return reserveDao_->insert(r, status);
}

Status ReservationService::cancelReservation(int reservationId) {
    return reserveDao_->updateStatus(reservationId, "cancelled");
}

vector<Reservation> ReservationService::getMyReservations(int userId) {
    return reserveDao_->findByUser(userId);
}

void ReservationService::processReturn(int bookId) {
    auto pending = reserveDao_->findByBook(bookId);
    if (!pending.empty()) {
        for (auto& r : pending) {
            if (r.getStatus() == "pending") {
                reserveDao_->updateStatus(r.getId(), "fulfilled");
                break;
            }
        }
    }
}

void ReservationService::processExpired() {
    auto expired = reserveDao_->findExpired(48);
    for (auto& r : expired)
        reserveDao_->updateStatus(r.getId(), "expired");
}
