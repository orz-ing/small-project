#ifndef OVERDUE_SCANNER_H
#define OVERDUE_SCANNER_H

#include <QObject>
#include <QTimer>
#include <QSqlDatabase>

class BorrowRecordDAO;
class UserDAO;
class FineCalculator;
class CreditEngine;

// 逾期扫描器：定时扫描逾期记录并自动扣分
class OverdueScanner : public QObject {
    Q_OBJECT
public:
    explicit OverdueScanner(QSqlDatabase db, QObject* parent = nullptr);
    ~OverdueScanner();

    void start();  // 启动定时扫描

signals:
    void overdueFound(int userId, int recordId, double fine);
    void scanCompleted(int count);

private slots:
    void scanOverdue();

private:
    QTimer* m_timer = nullptr;
    BorrowRecordDAO* m_borrowDao = nullptr;
    UserDAO* m_userDao = nullptr;
    FineCalculator* m_fineCalc = nullptr;
    CreditEngine* m_creditEngine = nullptr;
    QSqlDatabase m_db;
};

#endif
