#ifndef RECOMMEND_WIDGET_H
#define RECOMMEND_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>

class recommend_widget : public QWidget
{
    Q_OBJECT
public:
    explicit recommend_widget(QWidget *parent = nullptr);
    void setCurrentUserId(int userId);

private slots:
    void onBorrow();

private:
    void setupUI();
    void loadMockData();
    void updateTable();

    QTableWidget *m_table;
    QPushButton *m_borrowBtn;
    QLabel      *m_statusLabel;
    int m_currentUserId = 0;

    struct RecommendItem {
        QString title;
        QString author;
        QString reason;   // 推荐理由
    };
    QList<RecommendItem> m_items;
};

#endif // RECOMMEND_WIDGET_H