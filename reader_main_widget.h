#ifndef READER_MAIN_WIDGET_H
#define READER_MAIN_WIDGET_H
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include "models.h"
#include<QVBoxLayout>
#include "search_widget.h"
#include "borrow_panel.h"
#include "reservation_widget.h"
#include "recommend_widget.h"
class reader_main_widget : public QWidget {
    Q_OBJECT
public:
    explicit reader_main_widget(QWidget *parent = nullptr) {
        QVBoxLayout *lay = new QVBoxLayout(this);
        QLabel *title = new QLabel("读者界面");
        QTabWidget *tabs = new QTabWidget;
        tabs->addTab(new search_widget, "图书检索");
        tabs->addTab(new borrow_panel, "我的借阅");
        tabs->addTab(new recommend_widget, "智能推荐");
        tabs->addTab(new reservation_widget, "我的预约");
        lay->addWidget(title);
        lay->addWidget(tabs);
    }
    void setCurrentUser(const User &user) {
        // 待后续完善
    }
};

#endif