#ifndef RESERVATION_WIDGET_H
#define RESERVATION_WIDGET_H
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
class  reservation_widget: public QWidget
{
    Q_OBJECT
public:
    explicit reservation_widget(QWidget *parent = nullptr);
    void setCurrentUserId(int userId);

private slots:
    void onCancelReservation();

private:
    void setupUI();
    void loadMockData();
    void updateTable();

    QTableWidget *m_table;
    QPushButton *m_cancelBtn;
    QLabel      *m_statusLabel;
    int m_currentUserId = 0;

    struct ReservationItem {
        QString title;        // 书名
        QString reserveDate;  // 预约日期
        QString status;       // "排队中", "可取", "已过期"
        int queuePosition;    // 队列位置（仅排队中显示）
    };
    QList<ReservationItem> m_items;
};
#endif // RESERVATION_WIDGET_H
