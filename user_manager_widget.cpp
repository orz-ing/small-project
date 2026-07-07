#include "user_manager_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "main_window.h"
UserManagerWidget::UserManagerWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void UserManagerWidget::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);

    m_statusLabel = new QLabel("共 0 位用户");
    m_statusLabel->setStyleSheet("color: #666; padding: 2px;");
    lay->addWidget(m_statusLabel);

    m_table = new QTableWidget;
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"用户名", "角色", "状态", "信用分"});
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        "QTableWidget::item { color: #333333; }"
        );
    QHeaderView *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->resizeSection(1, 70);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->resizeSection(2, 80);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->resizeSection(3, 70);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addStretch();
    m_toggleBtn = new QPushButton("启用/禁用");
    m_toggleBtn->setEnabled(false);
    btnLay->addWidget(m_toggleBtn);
    lay->addLayout(btnLay);

    connect(m_toggleBtn, &QPushButton::clicked, this, &UserManagerWidget::onToggleDisable);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        m_toggleBtn->setEnabled(!m_table->selectedItems().isEmpty());
    });
}

void UserManagerWidget::loadMockData()
{
    m_users.clear();
    m_users.append({"reader1", "读者", false, 100});
    m_users.append({"reader2", "读者", false, 85});
    m_users.append({"admin", "管理员", false, 100});
    m_users.append({"disabled_user", "读者", true, 40});
    m_users.append({"reader3", "读者", false, 92});
    updateTable();
}

void UserManagerWidget::updateTable()
{
    m_table->setRowCount(0);
    for (const auto &user : m_users) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(user.username));
        m_table->setItem(row, 1, new QTableWidgetItem(user.role));
        QTableWidgetItem *statusItem = new QTableWidgetItem(user.disabled ? "已禁用" : "正常");
        statusItem->setForeground(user.disabled ? QColor("#d32f2f") : QColor("#388e3c"));
        m_table->setItem(row, 2, statusItem);
        QTableWidgetItem *creditItem = new QTableWidgetItem(QString::number(user.credit));
        if (user.credit < 60)
            creditItem->setForeground(QColor("#d32f2f"));
        m_table->setItem(row, 3, creditItem);
    }
    m_statusLabel->setText(QString("共 %1 位用户").arg(m_users.size()));
}

void UserManagerWidget::onToggleDisable()
{
    int row = m_table->currentRow();
    if (row < 0) return;
    m_users[row].disabled = !m_users[row].disabled;
    updateTable();
    MainWindow *mainWin = qobject_cast<MainWindow*>(window());
    if (mainWin) {
        QString msg = m_users[row].disabled ? "用户已禁用" : "用户已启用";
        mainWin->showNotification(msg, 1);   // 1 表示警告类型（橙色背景）
    }
}