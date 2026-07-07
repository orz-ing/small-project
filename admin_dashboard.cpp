#include "admin_dashboard.h"
#include "book_manager_widget.h"
#include "user_manager_widget.h"
#include "statistics_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

AdminDashboard::AdminDashboard(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void AdminDashboard::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航
    m_navList = new QListWidget;
    m_navList->addItem("图书管理");
    m_navList->addItem("用户管理");
    m_navList->addItem("数据统计");
    m_navList->setFixedWidth(180);
    m_navList->setCurrentRow(0);
    mainLayout->addWidget(m_navList);

    // 右侧内容区
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    m_welcomeLabel = new QLabel("管理员控制台");
    m_welcomeLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #333333; padding: 15px; background: white; border-bottom: 1px solid #ddd;"
        );
    rightLayout->addWidget(m_welcomeLabel);

    m_contentStack = new QStackedWidget;
    m_bookManager = new BookManagerWidget;
    m_userManager = new UserManagerWidget;
    m_statisticsWidget = new StatisticsWidget;

    m_contentStack->addWidget(m_bookManager);      // 索引0
    m_contentStack->addWidget(m_userManager);      // 索引1
    m_contentStack->addWidget(m_statisticsWidget); // 索引2

    rightLayout->addWidget(m_contentStack, 1);
    mainLayout->addLayout(rightLayout, 1);

    connect(m_navList, &QListWidget::currentRowChanged,
            m_contentStack, &QStackedWidget::setCurrentIndex);
}

void AdminDashboard::setCurrentUser(const User &user)
{
    m_welcomeLabel->setText(QString("管理员控制台 - 欢迎，%1").arg(user.username));
}