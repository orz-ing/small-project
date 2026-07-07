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
    void hideNotification();

private:
    void setupUI();
    void setupNotificationBar();

    QStackedWidget *m_pages;
    login_dialog      *m_loginPage;
    reader_main_widget *m_readerPage;
    admin_dashboard   *m_adminPage;

    QFrame  *m_notificationBar;
    QLabel  *m_notificationLabel;
    QTimer  *m_notifyTimer;
};
#endif // MAIN_WINDOW_H
