#include "borrow_service.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

BorrowService::BorrowService(QSqlDatabase db)
    : m_borrowDao(db), m_bookDao(db), m_userDao(db), m_db(db) {}

Result BorrowService::borrowBook(int userId, int bookId) {
    // 1. 检查用户状态
    User user = m_userDao.getById(userId);
    if (user.id == 0) return Result::fail("用户不存在");
    if (user.isDisabled) return Result::fail("账号已被禁用");
    if (m_creditEngine.isBlocked(user.creditScore))
        return Result::fail("信用分不足，无法借书（信用分 < " +
                           QString::number(CreditEngine::BLOCK_THRESHOLD) + "）");

    // 2. 获取图书信息（不使用事务，先做前置检查）
    Book book = m_bookDao.getById(bookId);
    if (book.id == 0) return Result::fail("图书不存在");
    if (book.availableStock <= 0) return Result::fail("库存不足");

    // 3. 检查是否已借阅该图书（防止重复借阅同一本）
    QVector<BorrowRecord> activeRecords = m_borrowDao.getActiveByUser(userId);
    for (const auto& rec : activeRecords) {
        if (rec.bookId == bookId) {
            return Result::fail("您已借阅过这本书，尚未归还");
        }
    }

    // 4. 乐观锁事务：借书
    m_db.transaction();

    // 重新读取，获取最新版本号（在事务内）
    QSqlQuery query(m_db);
    query.prepare("SELECT id, available_stock, version, title, isbn FROM books WHERE id = ?");
    query.addBindValue(bookId);
    query.exec();
    if (!query.next()) {
        m_db.rollback();
        return Result::fail("图书不存在");
    }
    int stock = query.value("available_stock").toInt();
    int version = query.value("version").toInt();
    QString bookTitle = query.value("title").toString();
    QString bookIsbn = query.value("isbn").toString();

    if (stock <= 0) {
        m_db.rollback();
        return Result::fail("库存不足");
    }

    // 乐观锁更新：只有version匹配时才扣减库存
    QSqlQuery updateQuery(m_db);
    updateQuery.prepare(
        "UPDATE books SET available_stock = available_stock - 1, "
        "version = version + 1, "
        "update_time = datetime('now','localtime') "
        "WHERE id = ? AND version = ?"
    );
    updateQuery.addBindValue(bookId);
    updateQuery.addBindValue(version);
    bool stockUpdated = updateQuery.exec() && updateQuery.numRowsAffected() > 0;

    if (!stockUpdated) {
        m_db.rollback();
        return Result::fail("并发冲突，请重试");
    }

    // 创建借阅记录（默认借阅30天）
    BorrowRecord record;
    record.userId = userId;
    record.bookId = bookId;
    record.bookTitle = bookTitle;
    record.bookIsbn = bookIsbn;
    record.borrowDate = QDateTime::currentDateTime();
    record.dueDate = record.borrowDate.addDays(30);

    int recordId = m_borrowDao.insert(record);
    if (recordId <= 0) {
        m_db.rollback();
        return Result::fail("创建借阅记录失败");
    }

    m_db.commit();
    return Result::ok("借书成功，请于 " + record.dueDate.toString("yyyy-MM-dd") + " 前归还");
}

Result BorrowService::returnBook(int recordId) {
    BorrowRecord record = m_borrowDao.getById(recordId);
    if (record.id == 0) return Result::fail("借阅记录不存在");
    if (record.status != BorrowStatus::Borrowing &&
        record.status != BorrowStatus::Overdue) {
        return Result::fail("该记录已归还");
    }

    m_db.transaction();

    QDateTime now = QDateTime::currentDateTime();

    // 计算罚款
    double fine = m_fineCalc.calculateFine(record.dueDate, now);

    // 更新借阅记录
    bool ok = m_borrowDao.returnBook(recordId, now, fine);
    if (!ok) {
        m_db.rollback();
        return Result::fail("归还失败");
    }

    // 增加库存
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE books SET available_stock = available_stock + 1, "
        "update_time = datetime('now','localtime') WHERE id = ?"
    );
    query.addBindValue(record.bookId);
    query.exec();

    // 更新用户信用分
    User user = m_userDao.getById(record.userId);
    if (fine > 0) {
        // 逾期：扣分
        int newScore = m_creditEngine.deduct(user.creditScore);
        m_userDao.updateCreditScore(record.userId, newScore);
    } else {
        // 按时：加分
        int newScore = m_creditEngine.reward(user.creditScore);
        m_userDao.updateCreditScore(record.userId, newScore);
    }

    m_db.commit();

    QString msg = "归还成功";
    if (fine > 0) {
        msg += QString("，产生罚款 %.1f 元").arg(fine);
    }
    return Result::ok(msg);
}

Result BorrowService::renewBook(int recordId) {
    BorrowRecord record = m_borrowDao.getById(recordId);
    if (record.id == 0) return Result::fail("借阅记录不存在");
    if (record.status != BorrowStatus::Borrowing) return Result::fail("该记录无法续借");
    if (record.returnDate.isValid()) return Result::fail("该书已归还，无法续借");

    // 检查是否有人预约了这本书
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM reservations WHERE book_id = ? AND status = 0");
    query.addBindValue(record.bookId);
    query.exec();
    if (query.next() && query.value(0).toInt() > 0) {
        return Result::fail("该书有预约排队，无法续借");
    }

    // 续借30天
    QDateTime newDueDate = record.dueDate.addDays(30);
    QSqlQuery update(m_db);
    update.prepare("UPDATE borrow_records SET due_date = ? WHERE id = ?");
    update.addBindValue(newDueDate.toString(Qt::ISODate));
    update.addBindValue(recordId);
    if (update.exec()) {
        return Result::ok("续借成功，新归还日期：" + newDueDate.toString("yyyy-MM-dd"));
    }
    return Result::fail("续借失败");
}

QVector<BorrowRecord> BorrowService::getUserRecords(int userId) const {
    return m_borrowDao.getByUser(userId);
}

QVector<BorrowRecord> BorrowService::getUserActiveRecords(int userId) const {
    return m_borrowDao.getActiveByUser(userId);
}

QVector<BorrowRecord> BorrowService::getOverdueRecords() const {
    return m_borrowDao.getOverdue();
}

int BorrowService::activeCount() const {
    return m_borrowDao.activeCount();
}

int BorrowService::overdueCount() const {
    return m_borrowDao.overdueCount();
}
