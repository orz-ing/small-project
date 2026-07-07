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

    /*m_loginPassword = new QLineEdit;
    m_loginPassword->setPlaceholderText("密码");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    loginLay->addWidget(m_loginPassword);*/
    // 登录密码框组合（与注册完全一致）
    QFrame *pwdFrame = new QFrame;
    pwdFrame->setFixedHeight(38);
    pwdFrame->setStyleSheet("QFrame { border: 1px solid #cccccc; border-radius: 4px; background: white; }");
    QHBoxLayout *pwdLayout = new QHBoxLayout(pwdFrame);
    pwdLayout->setContentsMargins(0, 0, 0, 0);
    pwdLayout->setSpacing(0);

    m_loginPassword = new QLineEdit;
    m_loginPassword->setPlaceholderText("密码");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    m_loginPassword->setStyleSheet("QLineEdit { border: none; background: transparent; padding: 0 10px; }");
    pwdLayout->addWidget(m_loginPassword);

    m_togglePasswordBtn = new QPushButton("显示");
    m_togglePasswordBtn->setFlat(true);
    m_togglePasswordBtn->setCursor(Qt::PointingHandCursor);
    m_togglePasswordBtn->setFixedSize(40, 36);
    m_togglePasswordBtn->setStyleSheet(
        "QPushButton { color: #1976D2; border: none; background: transparent; padding: 0px; font-size: 13px; }"
        "QPushButton:hover { color: #0D47A1; }"
        );
    connect(m_togglePasswordBtn, &QPushButton::clicked, this, [this]() {
        if (m_loginPassword->echoMode() == QLineEdit::Password) {
            m_loginPassword->setEchoMode(QLineEdit::Normal);
            m_togglePasswordBtn->setText("隐藏");
        } else {
            m_loginPassword->setEchoMode(QLineEdit::Password);
            m_togglePasswordBtn->setText("显示");
        }
    });
    pwdLayout->addWidget(m_togglePasswordBtn);

    loginLay->addWidget(pwdFrame);

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

    QFrame *regPwdFrame = new QFrame;
    regPwdFrame->setStyleSheet("QFrame { border: 1px solid #cccccc; border-radius: 4px; background: white; }");
    QHBoxLayout *regPwdLayout = new QHBoxLayout(regPwdFrame);
    regPwdLayout->setContentsMargins(0,0,0,0);
    regPwdLayout->setSpacing(0);
    // 注册密码框
    regPwdFrame->setFixedHeight(38);
    regPwdFrame->setStyleSheet("QFrame { border: 1px solid #cccccc; border-radius: 4px; background: white; }");
    regPwdLayout->setContentsMargins(0,0,0,0);
    regPwdLayout->setSpacing(0);

    m_regPassword = new QLineEdit;
    m_regPassword->setPlaceholderText("密码");
    m_regPassword->setEchoMode(QLineEdit::Password);
    m_regPassword->setStyleSheet("QLineEdit { border: none; background: transparent; padding: 0 10px; }");
    regPwdLayout->addWidget(m_regPassword);

    QPushButton *toggleRegPassBtn = new QPushButton("显示");
    toggleRegPassBtn->setFlat(true);
    toggleRegPassBtn->setCursor(Qt::PointingHandCursor);
    toggleRegPassBtn->setFixedSize(40, 36);
    toggleRegPassBtn->setStyleSheet(
        "QPushButton { color: #1976D2; border: none; background: transparent; padding: 0px; font-size: 13px; }"
        "QPushButton:hover { color: #0D47A1; }"
        );
    connect(toggleRegPassBtn, &QPushButton::clicked, this, [this, toggleRegPassBtn]() {
        if (m_regPassword->echoMode() == QLineEdit::Password) {
            m_regPassword->setEchoMode(QLineEdit::Normal);
            toggleRegPassBtn->setText("隐藏");
        } else {
            m_regPassword->setEchoMode(QLineEdit::Password);
            toggleRegPassBtn->setText("显示");
        }
    });
    regPwdLayout->addWidget(toggleRegPassBtn);
    regLay->addWidget(regPwdFrame);
    QFrame *confirmFrame = new QFrame;
    confirmFrame->setFixedHeight(38);
    confirmFrame->setStyleSheet("QFrame { border: 1px solid #cccccc; border-radius: 4px; background: white; }");
    QHBoxLayout *confirmLayout = new QHBoxLayout(confirmFrame);
    confirmLayout->setContentsMargins(0,0,0,0);
    confirmLayout->setSpacing(0);

    m_regConfirm = new QLineEdit;
    m_regConfirm->setPlaceholderText("确认密码");
    m_regConfirm->setEchoMode(QLineEdit::Password);
    m_regConfirm->setStyleSheet("QLineEdit { border: none; background: transparent; padding: 0 10px; }");
    confirmLayout->addWidget(m_regConfirm);

    QPushButton *toggleConfirmBtn = new QPushButton("显示");
    toggleConfirmBtn->setFlat(true);
    toggleConfirmBtn->setCursor(Qt::PointingHandCursor);
    toggleConfirmBtn->setFixedSize(40, 36);
    toggleConfirmBtn->setStyleSheet(
        "QPushButton { color: #1976D2; border: none; background: transparent; padding: 0px; font-size: 13px; }"
        "QPushButton:hover { color: #0D47A1; }"
        );
    connect(toggleConfirmBtn, &QPushButton::clicked, this, [this, toggleConfirmBtn]() {
        if (m_regConfirm->echoMode() == QLineEdit::Password) {
            m_regConfirm->setEchoMode(QLineEdit::Normal);
            toggleConfirmBtn->setText("隐藏");
        } else {
            m_regConfirm->setEchoMode(QLineEdit::Password);
            toggleConfirmBtn->setText("显示");
        }
    });
    confirmLayout->addWidget(toggleConfirmBtn);
    regLay->addWidget(confirmFrame);
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
/*
void login_dialog::togglePasswordVisibility()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;

    // 找到关联的输入框（遍历所有子控件，简单起见，直接用两个已知输入框判断）
    QLineEdit *target = nullptr;
    if (action->associatedWidgets().contains(m_loginPassword))
        target = m_loginPassword;
    else if (action->associatedWidgets().contains(m_regPassword))
        target = m_regPassword;
    else if (action->associatedWidgets().contains(m_regConfirm))
        target = m_regConfirm;

    if (!target) return;

    bool isPassword = (target->echoMode() == QLineEdit::Password);
    if (isPassword) {
        target->setEchoMode(QLineEdit::Normal);
        action->setIcon(QIcon(":/res/eye_open.png"));   // 有图标就换睁眼
        action->setText("🙈");  // 文字方案切换表情
    } else {
        target->setEchoMode(QLineEdit::Password);
        action->setIcon(QIcon(":/res/eye_closed.png"));
        action->setText("👁");
    }
}*/
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
