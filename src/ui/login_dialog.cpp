#include "login_dialog.h"
#include "bridge/api_bridge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QPainter>
#include <QAction>

// 用 QPainter 画眼睛图标，不依赖 emoji 字体
static QIcon makeEyeIcon(bool open) {
    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#5a6a7e"), 2));
    if (open) {
        // 眼睛轮廓
        p.drawEllipse(QPointF(12, 12), 7, 5);
        // 瞳孔
        p.setBrush(QColor("#5a6a7e"));
        p.drawEllipse(QPointF(12, 12), 2.5, 2.5);
    } else {
        // 闭眼：一条横线
        p.drawLine(QPointF(3, 12), QPointF(21, 12));
        // 小叉
        p.setPen(QPen(QColor("#e74c3c"), 2));
        p.drawLine(QPointF(17, 7), QPointF(10, 17));
    }
    p.end();
    return QIcon(pix);
}

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setupUI();
    setWindowTitle("图书管理系统 - 登录");
    setFixedSize(480, 580);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void LoginDialog::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    // 应用标题
    auto* titleLabel = new QLabel("📚 图书管理系统");
    titleLabel->setObjectName("sectionTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);

    m_stack = new QStackedWidget;
    mainLayout->addWidget(m_stack);

    m_statusLabel = new QLabel;
    m_statusLabel->setStyleSheet("color: #e74c3c; font-size: 13px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->hide();
    mainLayout->addWidget(m_statusLabel);

    // ====== 登录页 ======
    auto* loginPage = new QWidget;
    auto* loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setSpacing(12);

    auto* formLayout1 = new QFormLayout;
    m_loginUsername = new QLineEdit;
    m_loginUsername->setPlaceholderText("请输入用户名");
    formLayout1->addRow("用户名:", m_loginUsername);

    m_loginPassword = new QLineEdit;
    m_loginPassword->setEchoMode(QLineEdit::Password);
    m_loginPassword->setPlaceholderText("请输入密码");
    {
        QAction* act = m_loginPassword->addAction(makeEyeIcon(true), QLineEdit::TrailingPosition);
        QObject::connect(act, &QAction::triggered, m_loginPassword, [this, act]() {
            bool hidden = (m_loginPassword->echoMode() == QLineEdit::Password);
            m_loginPassword->setEchoMode(hidden ? QLineEdit::Normal : QLineEdit::Password);
            act->setIcon(makeEyeIcon(hidden));
        });
    }
    formLayout1->addRow("密  码:", m_loginPassword);
    loginLayout->addLayout(formLayout1);
    loginLayout->addSpacing(10);

    m_loginBtn = new QPushButton("登  录");
    m_loginBtn->setObjectName("primaryBtn");
    loginLayout->addWidget(m_loginBtn);

    m_toRegisterBtn = new QPushButton("没有账号？立即注册");
    m_toRegisterBtn->setFlat(true);
    m_toRegisterBtn->setStyleSheet("color: #3498db;");
    loginLayout->addWidget(m_toRegisterBtn);
    loginLayout->addStretch();

    // ====== 注册页 ======
    auto* registerPage = new QWidget;
    auto* regLayout = new QVBoxLayout(registerPage);
    regLayout->setSpacing(12);

    auto* formLayout2 = new QFormLayout;
    m_regUsername = new QLineEdit;
    m_regUsername->setPlaceholderText("请输入用户名");
    formLayout2->addRow("用户名:", m_regUsername);

    m_regPassword = new QLineEdit;
    m_regPassword->setEchoMode(QLineEdit::Password);
    m_regPassword->setPlaceholderText("至少6位密码");
    {
        QAction* act = m_regPassword->addAction(makeEyeIcon(true), QLineEdit::TrailingPosition);
        QObject::connect(act, &QAction::triggered, m_regPassword, [this, act]() {
            bool hidden = (m_regPassword->echoMode() == QLineEdit::Password);
            m_regPassword->setEchoMode(hidden ? QLineEdit::Normal : QLineEdit::Password);
            act->setIcon(makeEyeIcon(hidden));
        });
    }
    formLayout2->addRow("密  码:", m_regPassword);

    m_regConfirmPassword = new QLineEdit;
    m_regConfirmPassword->setEchoMode(QLineEdit::Password);
    m_regConfirmPassword->setPlaceholderText("再次输入密码");
    {
        QAction* act = m_regConfirmPassword->addAction(makeEyeIcon(true), QLineEdit::TrailingPosition);
        QObject::connect(act, &QAction::triggered, m_regConfirmPassword, [this, act]() {
            bool hidden = (m_regConfirmPassword->echoMode() == QLineEdit::Password);
            m_regConfirmPassword->setEchoMode(hidden ? QLineEdit::Normal : QLineEdit::Password);
            act->setIcon(makeEyeIcon(hidden));
        });
    }
    formLayout2->addRow("确认密码:", m_regConfirmPassword);

    m_regDisplayName = new QLineEdit;
    m_regDisplayName->setPlaceholderText("选填，显示用名称");
    formLayout2->addRow("显示名:", m_regDisplayName);
    regLayout->addLayout(formLayout2);

    auto* roleGroup = new QGroupBox("角色选择");
    auto* roleLayout = new QHBoxLayout(roleGroup);
    m_regRoleReader = new QRadioButton("读者");
    m_regRoleAdmin = new QRadioButton("管理员");
    m_regRoleReader->setChecked(true);
    roleLayout->addWidget(m_regRoleReader);
    roleLayout->addWidget(m_regRoleAdmin);
    regLayout->addWidget(roleGroup);
    regLayout->addSpacing(10);

    m_registerBtn = new QPushButton("注  册");
    m_registerBtn->setObjectName("successBtn");
    regLayout->addWidget(m_registerBtn);

    m_toLoginBtn = new QPushButton("已有账号？返回登录");
    m_toLoginBtn->setFlat(true);
    m_toLoginBtn->setStyleSheet("color: #3498db;");
    regLayout->addWidget(m_toLoginBtn);
    regLayout->addStretch();

    // 添加到堆叠
    m_stack->addWidget(loginPage);
    m_stack->addWidget(registerPage);
    m_stack->setCurrentIndex(0);

    // 信号连接
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(m_toRegisterBtn, &QPushButton::clicked, this, &LoginDialog::switchToRegister);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginDialog::onRegister);
    connect(m_toLoginBtn, &QPushButton::clicked, this, &LoginDialog::switchToLogin);

    // 回车登录
    connect(m_loginPassword, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
    connect(m_regConfirmPassword, &QLineEdit::returnPressed, this, &LoginDialog::onRegister);
}

void LoginDialog::onLogin() {
    QString username = m_loginUsername->text().trimmed();
    QString password = m_loginPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("请输入用户名和密码");
        m_statusLabel->show();
        return;
    }

    m_statusLabel->hide();
    Result result = ApiBridge::instance()->login(username, password);
    if (result.success) {
        accept();
    } else {
        m_statusLabel->setText(result.message);
        m_statusLabel->show();
    }
}

void LoginDialog::onRegister() {
    QString username = m_regUsername->text().trimmed();
    QString password = m_regPassword->text();
    QString confirm = m_regConfirmPassword->text();
    QString displayName = m_regDisplayName->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("请填写用户名和密码");
        m_statusLabel->show();
        return;
    }
    if (password != confirm) {
        m_statusLabel->setText("两次密码输入不一致");
        m_statusLabel->show();
        return;
    }
    if (password.length() < 6) {
        m_statusLabel->setText("密码长度不能少于6位");
        m_statusLabel->show();
        return;
    }

    m_statusLabel->hide();
    UserRole role = m_regRoleAdmin->isChecked() ? UserRole::Admin : UserRole::Reader;
    Result result = ApiBridge::instance()->registerUser(username, password, displayName, role);
    if (result.success) {
        QMessageBox::information(this, "注册成功", "注册成功，请登录");
        switchToLogin();
        m_loginUsername->setText(username);
    } else {
        m_statusLabel->setText(result.message);
        m_statusLabel->show();
    }
}

void LoginDialog::switchToRegister() {
    m_stack->setCurrentIndex(1);
    m_statusLabel->hide();
    setWindowTitle("图书管理系统 - 注册");
}

void LoginDialog::switchToLogin() {
    m_stack->setCurrentIndex(0);
    m_statusLabel->hide();
    setWindowTitle("图书管理系统 - 登录");
}

