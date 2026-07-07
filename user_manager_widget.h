#ifndef USER_MANAGER_WIDGET_H
#define USER_MANAGER_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>

class UserManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserManagerWidget(QWidget *parent = nullptr);

private slots:
    void onToggleDisable();

private:
    void setupUI();
    void loadMockData();
    void updateTable();

    QTableWidget *m_table;
    QPushButton *m_toggleBtn;
    QLabel      *m_statusLabel;

    struct UserItem {
        QString username;
        QString role;
        bool disabled;
        int credit;
    };
    QList<UserItem> m_users;
};

#endif