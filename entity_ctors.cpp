#include "backend.h"
User::User() : id_(0), credit_(100), status_("active"), role_("reader") {}
User::User(const string& username, const string& passwordHash, const string& realName, const string& role)
    : id_(0), username_(username), passwordHash_(passwordHash), realName_(realName),
      role_(role), credit_(100), status_("active") {}
bool User::setStatus(const string& s) {
    if (s != "active" && s != "disabled") return false;
    status_ = s; return true;
}
bool User::canBorrow() const { return credit_ >= 60 && status_ == "active"; }
int User::adjustCredit(int delta) { credit_ = max(0, min(100, credit_ + delta)); return credit_; }

Admin::Admin() : User(), adminLevel_(1) {}
Admin::Admin(const string& username, const string& passwordHash, const string& realName, int level)
    : User(username, passwordHash, realName, "admin"), adminLevel_(level) {}

Book::Book() : id_(0), categoryId_(0), stock_(0), totalStock_(0), version_(1) {}
int Book::adjustStock(int delta) {
    if (stock_ + delta < 0) return -1;
    stock_ += delta; return stock_;
}
BorrowRecord::BorrowRecord() : id_(0), userId_(0), bookId_(0), renewCount_(0) {}
bool BorrowRecord::setStatus(const string& s) {
    if (s != "borrowing" && s != "returned" && s != "overdue") return false;
    status_ = s; return true;
}
bool BorrowRecord::isOverdue(const string& today) const {
    return status_ == "borrowing" && dueDate_ < today;
}
string BorrowRecord::renew(int extraDays) {
    if (renewCount_ >= 2 || status_ != "borrowing") return "";
    // Parse date, add days, return new date (simplified)
    return "";
}
Reservation::Reservation() : id_(0), userId_(0), bookId_(0), priority_(0), status_("pending") {}
bool Reservation::setStatus(const string& s) {
    if (s != "pending" && s != "fulfilled" && s != "cancelled" && s != "expired") return false;
    status_ = s; return true;
}
bool Reservation::isExpired(const string& now) const { return expireDate_ < now; }
Category::Category() : id_(0), parentId_(0), level_(0) {}
LogEntry::LogEntry() : id_(0), adminId_(0) {}
