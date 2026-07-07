#include "backend.h"
string Status::getCodeName() const {
    switch (code) {
        case StatusCode::OK: return "OK";
        case StatusCode::ERR_DB_OPEN: return "ERR_DB_OPEN";
        case StatusCode::ERR_DB_EXEC: return "ERR_DB_EXEC";
        case StatusCode::ERR_USER_NOT_FOUND: return "ERR_USER_NOT_FOUND";
        case StatusCode::ERR_USER_DISABLED: return "ERR_USER_DISABLED";
        case StatusCode::ERR_USER_EXISTS: return "ERR_USER_EXISTS";
        case StatusCode::ERR_WRONG_PASSWORD: return "ERR_WRONG_PASSWORD";
        case StatusCode::ERR_BOOK_NOT_FOUND: return "ERR_BOOK_NOT_FOUND";
        case StatusCode::ERR_BOOK_OUT_OF_STOCK: return "ERR_BOOK_OUT_OF_STOCK";
        case StatusCode::ERR_BORROW_NOT_FOUND: return "ERR_BORROW_NOT_FOUND";
        case StatusCode::ERR_RENEW_LIMIT: return "ERR_RENEW_LIMIT";
        case StatusCode::ERR_RENEW_OVERDUE: return "ERR_RENEW_OVERDUE";
        case StatusCode::ERR_CREDIT_TOO_LOW: return "ERR_CREDIT_TOO_LOW";
        case StatusCode::ERR_VERSION_CONFLICT: return "ERR_VERSION_CONFLICT";
        case StatusCode::ERR_RESERVATION_EXISTS: return "ERR_RESERVATION_EXISTS";
        case StatusCode::ERR_RESERVATION_NOT_FOUND: return "ERR_RESERVATION_NOT_FOUND";
        case StatusCode::ERR_RESERVATION_EXPIRED: return "ERR_RESERVATION_EXPIRED";
        case StatusCode::ERR_PERMISSION_DENIED: return "ERR_PERMISSION_DENIED";
        case StatusCode::ERR_FILE_OPEN: return "ERR_FILE_OPEN";
        case StatusCode::ERR_FILE_WRITE: return "ERR_FILE_WRITE";
        default: return "ERR_UNKNOWN";
    }
}
