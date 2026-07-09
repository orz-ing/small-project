#include "main_window.h"
#include "bridge/api_bridge.h"
#include "ui/reader/reader_main_widget.h"
#include "ui/reader/search_widget.h"
#include "ui/reader/borrow_panel.h"
#include "ui/reader/recommend_widget.h"
#include "ui/reader/reservation_widget.h"
#include "ui/admin/admin_dashboard.h"
#include "ui/admin/book_manager_widget.h"
#include "ui/admin/user_manager_widget.h"
#include "ui/admin/statistics_widget.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    setWindowTitle("图书管理系统 - " + ApiBridge::instance()->currentUser().displayName);
    resize(1200, 800);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget;
    auto* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_sidebar = new QWidget;
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(200);
    auto* sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_userInfoLabel = new QLabel;
    m_userInfoLabel->setObjectName("titleLabel");
    auto user = ApiBridge::instance()->currentUser();
    m_userInfoLabel->setText("👤 " + user.displayName + (user.role == UserRole::Admin ? " (管理员)" : " (读者)"));
    sidebarLayout->addWidget(m_userInfoLabel);

    auto* sep = new QWidget;
    sep->setFixedHeight(1);
    sep->setStyleSheet("background-color: #34495e;");
    sidebarLayout->addWidget(sep);
    sidebarLayout->addSpacing(10);

    m_navLayout = new QVBoxLayout;
    m_navLayout->setSpacing(2);
    sidebarLayout->addLayout(m_navLayout);
    sidebarLayout->addStretch();

    auto* logoutBtn = new QPushButton("🚪 切换账号");
    logoutBtn->setObjectName("navBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);
    sidebarLayout->addWidget(logoutBtn);

    m_stackedWidget = new QStackedWidget;
    m_stackedWidget->setContentsMargins(20, 20, 20, 20);

    mainLayout->addWidget(m_sidebar);
    mainLayout->addWidget(m_stackedWidget, 1);
    setCentralWidget(centralWidget);

    if (ApiBridge::instance()->isAdmin())
        setupAdminPages();
    else
        setupReaderPages();

    if (!m_pages.isEmpty()) switchPage(0);
}

void MainWindow::setupReaderPages() {
    auto addPage = [this](const QString& lbl, QWidget* w) {
        m_navLabels.append(lbl);
        m_pages.append(w);
        m_stackedWidget->addWidget(w);
        auto* btn = new QPushButton(lbl);
        btn->setObjectName("navBtn");
        btn->setCheckable(true);
        m_navLayout->addWidget(btn);
        m_navButtons.append(btn);
        int idx = m_pages.size() - 1;
        connect(btn, &QPushButton::clicked, this, [this, idx]() { switchPage(idx); });
    };
    addPage("🔍 图书检索", new ReaderMainWidget);
    addPage("📖 搜索图书", new SearchWidget);
    addPage("📚 我的借阅", new BorrowPanel);
    addPage("⭐ 智能推荐", new RecommendWidget);
    addPage("📋 我的预约", new ReservationWidget);
}

void MainWindow::setupAdminPages() {
    auto addPage = [this](const QString& lbl, QWidget* w) {
        m_navLabels.append(lbl);
        m_pages.append(w);
        m_stackedWidget->addWidget(w);
        auto* btn = new QPushButton(lbl);
        btn->setObjectName("navBtn");
        btn->setCheckable(true);
        m_navLayout->addWidget(btn);
        m_navButtons.append(btn);
        int idx = m_pages.size() - 1;
        connect(btn, &QPushButton::clicked, this, [this, idx]() { switchPage(idx); });
    };
    addPage("📊 管理仪表盘", new AdminDashboard);
    addPage("📖 图书管理", new BookManagerWidget);
    addPage("👥 用户管理", new UserManagerWidget);
    addPage("📈 数据统计", new StatisticsWidget);
}

void MainWindow::switchPage(int index) {
    if (index < 0 || index >= m_pages.size()) return;
    m_stackedWidget->setCurrentWidget(m_pages[index]);
    updateNavButtons(index);

    // 切换到对应页面时刷新数据
    auto* page = m_pages[index];
    if (auto* bm = qobject_cast<BookManagerWidget*>(page)) {
        bm->refreshAll();
    } else if (auto* admin = qobject_cast<AdminDashboard*>(page)) {
        admin->refreshData();
    } else if (auto* st = qobject_cast<StatisticsWidget*>(page)) {
        st->refreshData();
    } else if (auto* bp = qobject_cast<BorrowPanel*>(page)) {
        bp->refreshData();
    }
}

void MainWindow::updateNavButtons(int index) {
    for (int i = 0; i < m_navButtons.size(); ++i)
        m_navButtons[i]->setChecked(i == index);
}

void MainWindow::onLogout() {
    if (QMessageBox::question(this, "切换账号", "确定要退出当前账号吗？",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        ApiBridge::instance()->logout();
        emit loggedOut();
        close();
    }
}
