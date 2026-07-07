#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include "models.h"
#include<QHBoxLayout>
class admin_dashboard : public QWidget {
    Q_OBJECT
public:
    explicit admin_dashboard(QWidget *parent = nullptr) {
        QHBoxLayout *lay = new QHBoxLayout(this);
        QListWidget *nav = new QListWidget;
        nav->addItem("图书管理");
        nav->addItem("用户管理");
        nav->addItem("数据统计");
        nav->setFixedWidth(180);

        QStackedWidget *stack = new QStackedWidget;
        stack->addWidget(new QLabel("图书管理"));
        stack->addWidget(new QLabel("用户管理"));
        stack->addWidget(new QLabel("数据统计"));

        connect(nav, &QListWidget::currentRowChanged, stack, &QStackedWidget::setCurrentIndex);

        lay->addWidget(nav);
        lay->addWidget(stack, 1);
    }
    void setCurrentUser(const User &user) {
        // 待完善
    }
};

#endif