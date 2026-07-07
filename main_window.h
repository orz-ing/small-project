#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QFrame>
#include <QLabel>
#include <QTimer>
#include "login_dialog.h"
#include "reader_main_widget.h"
#include "admin_dashboard.h"
#include "notification_widget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void showNotification(const QString &msg, int type = 0);

private slots:
    void switchToLogin();
    void switchToReader(const User &user);
    void switchToAdmin(const User &user);

private:
    void setupUI();
    void setupNotificationBar();

    QStackedWidget *m_pages=NULL;
    login_dialog      *m_loginPage=NULL;
    reader_main_widget *m_readerPage=NULL;
    AdminDashboard   *m_adminPage=NULL;

    NotificationWidget *m_notificationBar;

};
#endif // MAIN_WINDOW_H
