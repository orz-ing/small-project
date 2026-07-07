#ifndef BORROW_PANEL_H
#define BORROW_PANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>

class borrow_panel: public QWidget
{
    Q_OBJECT
public:
    explicit borrow_panel(QWidget *parent = nullptr);
    void setCurrentUserId(int userId);

private slots:
    void onReturnBook();
    void onRenewBook();

private:
    void setupUI();
    void loadMockData();
    void updateTable();

    QTableWidget *m_table;
    QPushButton *m_returnBtn;
    QPushButton *m_renewBtn;
    QLabel      *m_statusLabel;
    int m_currentUserId = 0;

    // 临时假数据结构
    struct BorrowItem {
        QString title;
        QString borrowDate;
        QString dueDate;
        QString status;   // "normal", "soon", "overdue"
        double  fine = 0.0;
    };
    QList<BorrowItem> m_items;

};

#endif // BORROW_PANEL_H
