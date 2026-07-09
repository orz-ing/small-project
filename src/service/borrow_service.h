#ifndef BORROW_SERVICE_H
#define BORROW_SERVICE_H

#include "models.h"
#include "dao/borrow_record_dao.h"
#include "dao/book_dao.h"
#include "dao/user_dao.h"
#include "engine/fine_calculator.h"
#include "engine/credit_engine.h"
#include <QSqlDatabase>

class BorrowService {
public:
    explicit BorrowService(QSqlDatabase db);

    // 借书（乐观锁事务）
    Result borrowBook(int userId, int bookId);

    // 还书
    Result returnBook(int recordId);

    // 续借
    Result renewBook(int recordId);

    // 获取用户借阅记录
    QVector<BorrowRecord> getUserRecords(int userId) const;

    // 获取用户当前借阅
    QVector<BorrowRecord> getUserActiveRecords(int userId) const;

    // 获取所有逾期记录
    QVector<BorrowRecord> getOverdueRecords() const;

    // 统计
    int activeCount() const;
    int overdueCount() const;

private:
    BorrowRecordDAO m_borrowDao;
    BookDAO m_bookDao;
    UserDAO m_userDao;
    FineCalculator m_fineCalc;
    CreditEngine m_creditEngine;
    QSqlDatabase m_db;
};
#endif
