#include "main_window.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),m_pages(nullptr),
    m_loginPage(nullptr),
    m_readerPage(nullptr),
    m_adminPage(nullptr)
{
    setupUI();
    switchToLogin();
}


void MainWindow::setupUI() {
    // 1. 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 2. 通知栏（先添加，默认隐藏）
    m_notificationBar = new NotificationWidget(this);
    mainLayout->addWidget(m_notificationBar);

    // 3. 页面堆栈（占据所有剩余空间）
    m_pages = new QStackedWidget(this);
    mainLayout->addWidget(m_pages, 1);  // 便于调试
    // 设置一个明显的背景色，确保可见
   // m_pages->setStyleSheet("QStackedWidget#mainStack { background-color: lightgreen; }");
    mainLayout->addWidget(m_pages, 1);    // 1 = stretch factor

    // 4. 设置中央部件（关键！）
    setCentralWidget(centralWidget);
}
/*
void MainWindow::setupNotificationBar() {
    m_notificationBar = new QFrame(this);
    m_notificationBar->setObjectName("notificationBar");
    m_notificationBar->setFixedHeight(45);
    m_notificationBar->hide();

    QHBoxLayout *barLay = new QHBoxLayout(m_notificationBar);
    barLay->setContentsMargins(15,0,15,0);

    m_notificationLabel = new QLabel(this);
    m_notificationLabel->setObjectName("notificationLabel");
    barLay->addWidget(m_notificationLabel, 1);

    m_notifyTimer = new QTimer(this);
    m_notifyTimer->setSingleShot(true);
    connect(m_notifyTimer, &QTimer::timeout, this, &MainWindow::hideNotification);
}
*/
void MainWindow::showNotification(const QString &msg, int type)
{
    NotificationWidget::Type t = NotificationWidget::Info;
    if (type == 1) t = NotificationWidget::Warning;
    else if (type == 2) t = NotificationWidget::Error;
    m_notificationBar->showMessage(msg, t);
}


void MainWindow::switchToLogin() {
    if (m_loginPage) {
        m_pages->removeWidget(m_loginPage);
        m_loginPage->deleteLater();
    }
    m_loginPage = new login_dialog(this);
    m_pages->addWidget(m_loginPage);
    m_pages->setCurrentWidget(m_loginPage);

    connect(m_loginPage, &login_dialog::loginSuccess, this, [this](const User &user) {
        if (user.role == "admin")
            switchToAdmin(user);
        else
            switchToReader(user);
    });
}
void MainWindow::switchToAdmin(const User &user)
{
    // 直接新建，不检查也不删除旧的，避免野指针引发崩溃或空白
    AdminDashboard *page = new AdminDashboard(this);
    page->setCurrentUser(user);
    m_pages->addWidget(page);
    m_pages->setCurrentWidget(page);
    showNotification(QString("管理员 %1 已登录").arg(user.username));
}
void MainWindow::switchToReader(const User &user)
{
    reader_main_widget *page = new reader_main_widget(this);
    page->setCurrentUser(user);
    m_pages->addWidget(page);
    m_pages->setCurrentWidget(page);
    showNotification(QString("欢迎读者：%1").arg(user.username));
}