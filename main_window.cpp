#include "main_window.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    switchToLogin();
}


void MainWindow::setupUI() {
    setWindowTitle("图书管理系统 - 奶龙崛起");
    resize(1100, 750);
    setMinimumSize(900, 600);

    QWidget *central = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(central);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);

    setupNotificationBar();
    lay->addWidget(m_notificationBar);

    m_pages = new QStackedWidget(this);
    lay->addWidget(m_pages, 1);
    setCentralWidget(central);
}

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

void MainWindow::showNotification(const QString &msg, int type) {
    QString bg;
    if (type == 1) bg = "#f57c00";       // warning
    else if (type == 2) bg = "#d32f2f";  // error
    else bg = "#323232";                 // info

    m_notificationBar->setStyleSheet(
        QString("QFrame#notificationBar { background-color: %1; border-radius: 4px; }").arg(bg));
    m_notificationLabel->setText(msg);
    m_notificationBar->show();
    m_notifyTimer->start(3000);
}

void MainWindow::hideNotification() {
    m_notificationBar->hide();
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

void MainWindow::switchToReader(const User &user) {
    if (m_readerPage) {
        m_pages->removeWidget(m_readerPage);
        m_readerPage->deleteLater();
    }
    m_readerPage = new reader_main_widget(this);
    m_readerPage->setCurrentUser(user);
    m_pages->addWidget(m_readerPage);
    m_pages->setCurrentWidget(m_readerPage);
    showNotification(QString("欢迎读者：%1").arg(user.username));
}

void MainWindow::switchToAdmin(const User &user) {
    if (m_adminPage) {
        m_pages->removeWidget(m_adminPage);
        m_adminPage->deleteLater();
    }
    m_adminPage = new admin_dashboard(this);
    m_adminPage->setCurrentUser(user);
    m_pages->addWidget(m_adminPage);
    m_pages->setCurrentWidget(m_adminPage);
    showNotification(QString("管理员 %1 已登录").arg(user.username));
}