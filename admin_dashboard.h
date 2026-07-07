#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include "models.h"

class BookManagerWidget;
class UserManagerWidget;
class StatisticsWidget;

class AdminDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget *parent = nullptr);
    void setCurrentUser(const User &user);

private:
    void setupUI();

    QListWidget *m_navList;
    QStackedWidget *m_contentStack;
    QLabel *m_welcomeLabel;

    BookManagerWidget *m_bookManager;
    UserManagerWidget *m_userManager;
    StatisticsWidget *m_statisticsWidget;
};

#endif