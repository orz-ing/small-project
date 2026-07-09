#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include <QGroupBox>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);

private slots:
    void onLogin();
    void onRegister();
    void switchToRegister();
    void switchToLogin();

private:
    void setupUI();

    QStackedWidget* m_stack;

    // 登录页
    QLineEdit* m_loginUsername;
    QLineEdit* m_loginPassword;
    QPushButton* m_loginBtn;
    QPushButton* m_toRegisterBtn;

    // 注册页
    QLineEdit* m_regUsername;
    QLineEdit* m_regPassword;
    QLineEdit* m_regConfirmPassword;
    QLineEdit* m_regDisplayName;
    QRadioButton* m_regRoleReader;
    QRadioButton* m_regRoleAdmin;
    QPushButton* m_registerBtn;
    QPushButton* m_toLoginBtn;

    QLabel* m_statusLabel;
};

#endif
