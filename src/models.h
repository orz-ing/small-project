#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

// 日志级别
enum class LogLevel {
    Info,
    Warning,
    Error
};

// 用户角色
enum class UserRole {
    Admin,
    Reader
};

// 借阅记录状态
enum class BorrowStatus {
    Borrowing,   // 借阅中
    Returned,    // 已归还
    Overdue,     // 逾期
    Lost         // 丢失
};

// 预约状态
enum class ReservationStatus {
    Pending,     // 等待中
    Notified,    // 已通知（有书可借）
    Cancelled,   // 已取消
    Expired,     // 已过期
    Fulfilled    // 已完成（已借出）
};

// 通知类型
enum class NotificationType {
    BookDue,         // 图书即将到期
    BookOverdue,     // 图书已逾期
    ReservationAvailable, // 预约到书
    FineIncurred,    // 产生罚款
    SystemMessage    // 系统消息
};

// 分类
struct Category {
    int id = 0;
    int parentId = -1;
    QString name;
    QString path;       // 完整路径，如 "文学/小说/科幻"
    int level = 0;      // 层级深度

    QJsonObject toJson() const;
    static Category fromJson(const QJsonObject& obj);
};

// 图书
struct Book {
    int id = 0;
    QString isbn;
    QString title;
    QString author;
    QString publisher;
    int categoryId = 0;
    QString categoryPath;       // 冗余，方便显示
    int totalStock = 0;
    int availableStock = 0;
    int version = 0;            // 乐观锁版本号
    QString coverPath;          // 封面图片路径（可选）
    QString description;
    QDateTime createTime;
    QDateTime updateTime;

    QJsonObject toJson() const;
    static Book fromJson(const QJsonObject& obj);
};

// 用户
struct User {
    int id = 0;
    QString username;
    QString passwordHash;
    QString displayName;
    UserRole role = UserRole::Reader;
    int creditScore = 100;       // 信用分，初始100
    bool isDisabled = false;
    QString email;
    QString phone;
    QDateTime createTime;

    QJsonObject toJson() const;
    static User fromJson(const QJsonObject& obj);
};

// 借阅记录
struct BorrowRecord {
    int id = 0;
    int userId = 0;
    int bookId = 0;
    QString bookTitle;           // 冗余
    QString bookIsbn;            // 冗余
    QDateTime borrowDate;
    QDateTime dueDate;
    QDateTime returnDate;        // 为空表示未还
    double fine = 0.0;           // 已产生的罚款
    BorrowStatus status = BorrowStatus::Borrowing;

    QJsonObject toJson() const;
    static BorrowRecord fromJson(const QJsonObject& obj);
};

// 预约
struct Reservation {
    int id = 0;
    int userId = 0;
    int bookId = 0;
    QString bookTitle;           // 冗余
    QDateTime reserveDate;
    QDateTime expireDate;        // 过期时间（通常48小时）
    int priority = 0;            // 优先级，默认0
    ReservationStatus status = ReservationStatus::Pending;
    int queuePosition = 0;       // 队列位置

    QJsonObject toJson() const;
    static Reservation fromJson(const QJsonObject& obj);
};

// 日志条目
struct LogEntry {
    int id = 0;
    int adminId = 0;
    QString adminName;           // 冗余
    QString action;              // 如 "delete_book", "disable_user"
    QString targetType;          // "book", "user", "borrow"
    int targetId = 0;
    QString detail;              // 详细描述
    QDateTime timestamp;

    QJsonObject toJson() const;
    static LogEntry fromJson(const QJsonObject& obj);
};

// 统计结果
struct Statistics {
    int totalBooks = 0;
    int totalUsers = 0;
    int activeBorrows = 0;       // 当前借阅中
    int overdueCount = 0;        // 逾期数量
    double totalFine = 0.0;      // 总罚款
    int todayBorrows = 0;        // 今日借阅
    int todayReturns = 0;        // 今日归还

    // 分时统计（用于图表）
    struct MonthlyStat {
        int month;               // 格式 202401
        int borrowCount;
        int returnCount;
    };
    QVector<MonthlyStat> monthlyStats;

    // 分类统计（用于饼图）
    struct CategoryStat {
        int categoryId;
        QString categoryName;
        int bookCount;
    };
    QVector<CategoryStat> categoryStats;

    // 热门图书（用于柱状图TOP10）
    struct HotBook {
        int bookId;
        QString bookTitle;
        int borrowCount;
    };
    QVector<HotBook> hotBooks;
};

// 操作结果
struct Result {
    bool success = false;
    QString message;
    QVariant data;               // 可选的返回数据

    static Result ok(const QString& msg = QString(), const QVariant& data = QVariant()) {
        return Result{true, msg, data};
    }
    static Result fail(const QString& msg) {
        return Result{false, msg, QVariant()};
    }
};

// ============ Json序列化实现（inline避免链接问题） ============

inline QJsonObject Category::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["parentId"] = parentId;
    obj["name"] = name;
    obj["path"] = path;
    obj["level"] = level;
    return obj;
}

inline Category Category::fromJson(const QJsonObject& obj) {
    Category c;
    c.id = obj["id"].toInt();
    c.parentId = obj["parentId"].toInt(-1);
    c.name = obj["name"].toString();
    c.path = obj["path"].toString();
    c.level = obj["level"].toInt();
    return c;
}

inline QJsonObject Book::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["isbn"] = isbn;
    obj["title"] = title;
    obj["author"] = author;
    obj["publisher"] = publisher;
    obj["categoryId"] = categoryId;
    obj["categoryPath"] = categoryPath;
    obj["totalStock"] = totalStock;
    obj["availableStock"] = availableStock;
    obj["version"] = version;
    obj["coverPath"] = coverPath;
    obj["description"] = description;
    obj["createTime"] = createTime.toString(Qt::ISODate);
    obj["updateTime"] = updateTime.toString(Qt::ISODate);
    return obj;
}

inline Book Book::fromJson(const QJsonObject& obj) {
    Book b;
    b.id = obj["id"].toInt();
    b.isbn = obj["isbn"].toString();
    b.title = obj["title"].toString();
    b.author = obj["author"].toString();
    b.publisher = obj["publisher"].toString();
    b.categoryId = obj["categoryId"].toInt();
    b.categoryPath = obj["categoryPath"].toString();
    b.totalStock = obj["totalStock"].toInt();
    b.availableStock = obj["availableStock"].toInt();
    b.version = obj["version"].toInt();
    b.coverPath = obj["coverPath"].toString();
    b.description = obj["description"].toString();
    b.createTime = QDateTime::fromString(obj["createTime"].toString(), Qt::ISODate);
    b.updateTime = QDateTime::fromString(obj["updateTime"].toString(), Qt::ISODate);
    return b;
}

inline QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["username"] = username;
    obj["displayName"] = displayName;
    obj["role"] = static_cast<int>(role);
    obj["creditScore"] = creditScore;
    obj["isDisabled"] = isDisabled;
    obj["email"] = email;
    obj["phone"] = phone;
    obj["createTime"] = createTime.toString(Qt::ISODate);
    return obj;
}

inline User User::fromJson(const QJsonObject& obj) {
    User u;
    u.id = obj["id"].toInt();
    u.username = obj["username"].toString();
    u.displayName = obj["displayName"].toString();
    u.role = static_cast<UserRole>(obj["role"].toInt());
    u.creditScore = obj["creditScore"].toInt(100);
    u.isDisabled = obj["isDisabled"].toBool(false);
    u.email = obj["email"].toString();
    u.phone = obj["phone"].toString();
    u.createTime = QDateTime::fromString(obj["createTime"].toString(), Qt::ISODate);
    return u;
}

inline QJsonObject BorrowRecord::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["userId"] = userId;
    obj["bookId"] = bookId;
    obj["bookTitle"] = bookTitle;
    obj["bookIsbn"] = bookIsbn;
    obj["borrowDate"] = borrowDate.toString(Qt::ISODate);
    obj["dueDate"] = dueDate.toString(Qt::ISODate);
    obj["returnDate"] = returnDate.isValid() ? returnDate.toString(Qt::ISODate) : QString();
    obj["fine"] = fine;
    obj["status"] = static_cast<int>(status);
    return obj;
}

inline BorrowRecord BorrowRecord::fromJson(const QJsonObject& obj) {
    BorrowRecord r;
    r.id = obj["id"].toInt();
    r.userId = obj["userId"].toInt();
    r.bookId = obj["bookId"].toInt();
    r.bookTitle = obj["bookTitle"].toString();
    r.bookIsbn = obj["bookIsbn"].toString();
    r.borrowDate = QDateTime::fromString(obj["borrowDate"].toString(), Qt::ISODate);
    r.dueDate = QDateTime::fromString(obj["dueDate"].toString(), Qt::ISODate);
    QString rd = obj["returnDate"].toString();
    if (!rd.isEmpty()) r.returnDate = QDateTime::fromString(rd, Qt::ISODate);
    r.fine = obj["fine"].toDouble();
    r.status = static_cast<BorrowStatus>(obj["status"].toInt());
    return r;
}

inline QJsonObject Reservation::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["userId"] = userId;
    obj["bookId"] = bookId;
    obj["bookTitle"] = bookTitle;
    obj["reserveDate"] = reserveDate.toString(Qt::ISODate);
    obj["expireDate"] = expireDate.toString(Qt::ISODate);
    obj["priority"] = priority;
    obj["status"] = static_cast<int>(status);
    obj["queuePosition"] = queuePosition;
    return obj;
}

inline Reservation Reservation::fromJson(const QJsonObject& obj) {
    Reservation r;
    r.id = obj["id"].toInt();
    r.userId = obj["userId"].toInt();
    r.bookId = obj["bookId"].toInt();
    r.bookTitle = obj["bookTitle"].toString();
    r.reserveDate = QDateTime::fromString(obj["reserveDate"].toString(), Qt::ISODate);
    r.expireDate = QDateTime::fromString(obj["expireDate"].toString(), Qt::ISODate);
    r.priority = obj["priority"].toInt();
    r.status = static_cast<ReservationStatus>(obj["status"].toInt());
    r.queuePosition = obj["queuePosition"].toInt();
    return r;
}

inline QJsonObject LogEntry::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["adminId"] = adminId;
    obj["adminName"] = adminName;
    obj["action"] = action;
    obj["targetType"] = targetType;
    obj["targetId"] = targetId;
    obj["detail"] = detail;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);
    return obj;
}

inline LogEntry LogEntry::fromJson(const QJsonObject& obj) {
    LogEntry e;
    e.id = obj["id"].toInt();
    e.adminId = obj["adminId"].toInt();
    e.adminName = obj["adminName"].toString();
    e.action = obj["action"].toString();
    e.targetType = obj["targetType"].toString();
    e.targetId = obj["targetId"].toInt();
    e.detail = obj["detail"].toString();
    e.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    return e;
}

#endif // MODELS_H
