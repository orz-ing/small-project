#include "overdue_scanner.h"
#include "dao/borrow_record_dao.h"
#include "dao/user_dao.h"
#include "engine/fine_calculator.h"
#include "engine/credit_engine.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

OverdueScanner::OverdueScanner(QSqlDatabase db, QObject* parent)
    : QObject(parent), m_db(db) {
    m_borrowDao = new BorrowRecordDAO(db);
    m_userDao = new UserDAO(db);
    m_fineCalc = new FineCalculator();
    m_creditEngine = new CreditEngine();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &OverdueScanner::scanOverdue);
}

OverdueScanner::~OverdueScanner() {
    delete m_borrowDao;
    delete m_userDao;
    delete m_fineCalc;
    delete m_creditEngine;
}

void OverdueScanner::start() {
    // 启动时立即执行一次
    scanOverdue();

    // 每隔1小时检查一次（实际可用更频繁）
    m_timer->start(3600000); // 1 hour
}

void OverdueScanner::scanOverdue() {
    auto overdueRecords = m_borrowDao->getOverdue();
    int count = 0;

    for (const auto& record : overdueRecords) {
        // 如果已经是逾期状态，跳过（只处理状态为Borrowing的记录）
        if (record.status == BorrowStatus::Overdue) continue;
        if (record.status != BorrowStatus::Borrowing) continue;

        // 更新状态为逾期
        QSqlQuery query(m_db);
        query.prepare("UPDATE borrow_records SET status = ? WHERE id = ?");
        query.addBindValue(static_cast<int>(BorrowStatus::Overdue));
        query.addBindValue(record.id);
        query.exec();

        // 如果产生了罚款，更新
        double fine = m_fineCalc->calculateFine(record.dueDate);
        if (fine > 0) {
            QSqlQuery fineQuery(m_db);
            fineQuery.prepare("UPDATE borrow_records SET fine = ? WHERE id = ?");
            fineQuery.addBindValue(fine);
            fineQuery.addBindValue(record.id);
            fineQuery.exec();
        }

        // 扣信用分
        User user = m_userDao->getById(record.userId);
        if (user.id > 0) {
            int newScore = m_creditEngine->deduct(user.creditScore);
            m_userDao->updateCreditScore(record.userId, newScore);
            emit overdueFound(record.userId, record.id, fine);
        }

        count++;
    }

    if (count > 0) {
        qInfo() << "Overdue scan: processed" << count << "records";
    }
    emit scanCompleted(count);
}
