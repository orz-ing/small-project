#include "user_manager_widget.h"
#include "bridge/api_bridge.h"
#include "dao/user_dao.h"
#include "dao/database_manager.h"
#include "engine/credit_engine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

UserManagerWidget::UserManagerWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void UserManagerWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("👥 用户管理");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* toolLayout = new QHBoxLayout;
    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText("搜索用户名...");
    toolLayout->addWidget(m_searchInput, 1);

    m_toggleBtn = new QPushButton("🔒 禁用/启用");
    m_toggleBtn->setObjectName("warningBtn");
    toolLayout->addWidget(m_toggleBtn);

    auto* refreshBtn = new QPushButton("🔄 刷新");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &UserManagerWidget::refreshUserList);
    toolLayout->addWidget(refreshBtn);
    layout->addLayout(toolLayout);

    m_userTable = new QTableWidget;
    m_userTable->setColumnCount(7);
    m_userTable->setHorizontalHeaderLabels({"ID", "用户名", "显示名", "角色", "信用分", "状态", "注册时间"});
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_userTable->setColumnHidden(0, true);
    m_userTable->verticalHeader()->hide();
    m_userTable->setAlternatingRowColors(true);
    m_userTable->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_userTable, 1);

    connect(m_toggleBtn, &QPushButton::clicked, this, &UserManagerWidget::onToggleDisable);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &UserManagerWidget::onSearch);

    refreshUserList();
}

void UserManagerWidget::refreshUserList() {
    auto users = ApiBridge::instance()->getAllUsers();
    m_userTable->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        auto& u = users[i];
        m_userTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
        m_userTable->setItem(i, 1, new QTableWidgetItem(u.username));
        m_userTable->setItem(i, 2, new QTableWidgetItem(u.displayName));
        m_userTable->setItem(i, 3, new QTableWidgetItem(u.role == UserRole::Admin ? "管理员" : "读者"));
        m_userTable->setItem(i, 4, new QTableWidgetItem(QString::number(u.creditScore)));
        m_userTable->setItem(i, 5, new QTableWidgetItem(u.isDisabled ? "🔒 已禁用" : "✅ 正常"));
        m_userTable->setItem(i, 6, new QTableWidgetItem(u.createTime.toString("yyyy-MM-dd")));

        if (u.isDisabled) {
            m_userTable->item(i, 5)->setForeground(Qt::red);
        }
        // 信用分颜色
        CreditEngine ce;
        if (ce.isBlocked(u.creditScore)) {
            m_userTable->item(i, 4)->setForeground(Qt::red);
        }
    }
    m_userTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void UserManagerWidget::onToggleDisable() {
    int row = m_userTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择一个用户"); return; }

    auto idItem = m_userTable->item(row, 0);
    int userId = idItem->text().toInt();
    auto statusItem = m_userTable->item(row, 5);
    bool currentlyDisabled = statusItem->text().contains("禁用");

    auto reply = QMessageBox::question(this, "确认",
        currentlyDisabled ? "确定要启用该用户吗？" : "确定要禁用该用户吗？");
    if (reply != QMessageBox::Yes) return;

    Result r = ApiBridge::instance()->disableUser(userId, !currentlyDisabled);
    if (r.success) {
        QMessageBox::information(this, "成功", r.message);
        refreshUserList();
    } else {
        QMessageBox::warning(this, "失败", r.message);
    }
}

void UserManagerWidget::onSearch() {
    QString keywords = m_searchInput->text().trimmed();
    if (keywords.isEmpty()) { refreshUserList(); return; }

    auto users = ApiBridge::instance()->getAllUsers();
    QVector<User> filtered;
    for (const auto& u : users) {
        if (u.username.contains(keywords, Qt::CaseInsensitive) ||
            u.displayName.contains(keywords, Qt::CaseInsensitive)) {
            filtered.append(u);
        }
    }

    m_userTable->setRowCount(filtered.size());
    for (int i = 0; i < filtered.size(); ++i) {
        auto& u = filtered[i];
        m_userTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
        m_userTable->setItem(i, 1, new QTableWidgetItem(u.username));
        m_userTable->setItem(i, 2, new QTableWidgetItem(u.displayName));
        m_userTable->setItem(i, 3, new QTableWidgetItem(u.role == UserRole::Admin ? "管理员" : "读者"));
        m_userTable->setItem(i, 4, new QTableWidgetItem(QString::number(u.creditScore)));
        m_userTable->setItem(i, 5, new QTableWidgetItem(u.isDisabled ? "🔒 已禁用" : "✅ 正常"));
        m_userTable->setItem(i, 6, new QTableWidgetItem(u.createTime.toString("yyyy-MM-dd")));
    }
    m_userTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}
