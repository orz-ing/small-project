#include "login_dialog.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHBoxLayout>
login_dialog::login_dialog(QWidget *parent)
    : QDialog(parent)

{
    setWindowTitle("图书管理系统 - 登录/注册");
    setFixedSize(420, 450);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);

    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack);

    // ==================== 登录页 ====================
    QWidget *loginPage = new QWidget;
    QVBoxLayout *loginLay = new QVBoxLayout(loginPage);
    loginLay->setSpacing(12);

    QLabel *loginTitle = new QLabel("用户登录");
    loginTitle->setAlignment(Qt::AlignCenter);
    loginTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2;");
    loginLay->addWidget(loginTitle);

    m_loginUsername = new QLineEdit;
    m_loginUsername->setPlaceholderText("用户名");
    loginLay->addWidget(m_loginUsername);

    m_loginPassword = new QLineEdit;
    m_loginPassword->setPlaceholderText("密码");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    loginLay->addWidget(m_loginPassword);

    m_loginButton = new QPushButton("登 录");
    loginLay->addWidget(m_loginButton);

    QPushButton *toRegisterBtn = new QPushButton("注 册");
    loginLay->addWidget(toRegisterBtn);

    m_loginStatus = new QLabel;
    m_loginStatus->setAlignment(Qt::AlignCenter);
    m_loginStatus->setStyleSheet("color: red; font-size: 12px;");
    loginLay->addWidget(m_loginStatus);

    m_stack->addWidget(loginPage);

    // ==================== 注册页 ====================
    QWidget *regPage = new QWidget;
    QVBoxLayout *regLay = new QVBoxLayout(regPage);
    regLay->setSpacing(12);

    QLabel *regTitle = new QLabel("注册新账号");
    regTitle->setAlignment(Qt::AlignCenter);
    regTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #1976D2;");
    regLay->addWidget(regTitle);

    m_regUsername = new QLineEdit;
    m_regUsername->setPlaceholderText("用户名");
    regLay->addWidget(m_regUsername);

    m_regPassword = new QLineEdit;
    m_regPassword->setPlaceholderText("密码");
    m_regPassword->setEchoMode(QLineEdit::Password);
    regLay->addWidget(m_regPassword);

    m_regConfirm = new QLineEdit;
    m_regConfirm->setPlaceholderText("确认密码");
    m_regConfirm->setEchoMode(QLineEdit::Password);
    regLay->addWidget(m_regConfirm);

    m_regRole = new QComboBox;
    m_regRole->addItem("读者", "reader");
    m_regRole->addItem("管理员", "admin");
    regLay->addWidget(m_regRole);

    m_regButton = new QPushButton("注 册");
    regLay->addWidget(m_regButton);

    // ---- 用 QLabel 做“返回登录”链接 ----
    QLabel *toLoginLink = new QLabel("<a href='#' style='color:#1976D2; text-decoration:none; font-size:13px;'>返回登录</a>");
    toLoginLink->setAlignment(Qt::AlignCenter);
    toLoginLink->setCursor(Qt::PointingHandCursor);
    toLoginLink->installEventFilter(this);   // 安装事件过滤器，让 QLabel 可以响应点击
    regLay->addWidget(toLoginLink);

    m_regStatus = new QLabel;
    m_regStatus->setAlignment(Qt::AlignCenter);
    m_regStatus->setStyleSheet("color: red; font-size: 12px;");
    regLay->addWidget(m_regStatus);

    m_stack->addWidget(regPage);

    // 默认显示登录页
    m_stack->setCurrentIndex(0);

    // ==================== 信号连接 ====================
    connect(m_loginButton, &QPushButton::clicked, this, &login_dialog::onLoginClicked);
    connect(toRegisterBtn, &QPushButton::clicked, this, &login_dialog::showRegisterPage);
    connect(m_regButton, &QPushButton::clicked, this, &login_dialog::onRegisterClicked);
}
// ---------- 页面切换 ----------
void login_dialog::showLoginPage() {
    m_stack->setCurrentIndex(0);
    // 清空状态提示
    m_loginStatus->clear();
    m_regStatus->clear();
}

bool login_dialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *label = qobject_cast<QLabel*>(obj);
        if (label && label->text().contains("返回登录")) {
            showLoginPage();
            return true;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void login_dialog::showRegisterPage() {
    m_stack->setCurrentIndex(1);
    m_loginStatus->clear();
    m_regStatus->clear();
}

// ---------- 模拟登录 ----------
void login_dialog::onLoginClicked() {
    QString username = m_loginUsername->text().trimmed();
    QString password = m_loginPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_loginStatus->setText("用户名和密码不能为空");
        return;
    }

    // 临时模拟：admin 是管理员，其他全是读者
    User user;
    user.username = username;
    user.role = (username == "admin") ? "admin" : "reader";

    emit loginSuccess(user);
    accept();  // 关闭登录对话框
}

// ---------- 模拟注册 ----------
void login_dialog::onRegisterClicked() {
    QString username = m_regUsername->text().trimmed();
    QString password = m_regPassword->text();
    QString confirm  = m_regConfirm->text();

    if (username.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        m_regStatus->setText("所有字段不能为空");
        return;
    }
    if (password != confirm) {
        m_regStatus->setText("两次密码不一致");
        return;
    }

    // 临时模拟：注册后直接视为登录成功，角色按选择的来
    User user;
    user.username = username;
    user.role = m_regRole->currentData().toString();  // "reader" 或 "admin"

    emit loginSuccess(user);
    accept();
}
login_dialog::~login_dialog()
{

}
