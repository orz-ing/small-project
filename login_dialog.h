#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H
#include"models.h"

#include <QDialog>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include<QEvent>
namespace Ui {
class login_dialog;
}

class login_dialog : public QDialog
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
public:
    explicit login_dialog(QWidget *parent = nullptr);
    ~login_dialog();
signals:
    void loginSuccess(const User &user);
private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void showLoginPage();
    void showRegisterPage();
    //void onReturnToLoginClicked();
private:
    QStackedWidget *m_stack;
    // 登录页控件
    QLineEdit   *m_loginUsername;
    QLineEdit   *m_loginPassword;
    QPushButton *m_loginButton;
    QLabel      *m_loginStatus;

    // 注册页控件
    QLineEdit   *m_regUsername;
    QLineEdit   *m_regPassword;
    QLineEdit   *m_regConfirm;
    QComboBox   *m_regRole;        // 读者/管理员
    QPushButton *m_regButton;
    QLabel      *m_regStatus;
};

#endif // LOGIN_DIALOG_H
