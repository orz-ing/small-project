#include "reservation_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>

reservation_widget::reservation_widget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void reservation_widget::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(6);

    // 状态标签
    m_statusLabel = new QLabel("共 0 条预约记录");
    m_statusLabel->setStyleSheet("color: #666; padding: 0px; margin-left: 2px;");
    lay->addWidget(m_statusLabel);

    // 表格
    m_table = new QTableWidget;
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"书名", "预约日期", "状态", "队列位置"});

    // 保证文字可见（沿用之前的有效设置）
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        "QTableWidget::item { color: #333333; }"
        );

    QHeaderView *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);    // 书名拉伸
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->resizeSection(1, 110);   // 预约日期
    header->resizeSection(2, 80);    // 状态
    header->resizeSection(3, 80);    // 队列位置

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    // 按钮区
    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addStretch();
    m_cancelBtn = new QPushButton("取消预约");
    m_cancelBtn->setEnabled(false);
    btnLay->addWidget(m_cancelBtn);
    lay->addLayout(btnLay);

    connect(m_cancelBtn, &QPushButton::clicked, this, &reservation_widget::onCancelReservation);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        m_cancelBtn->setEnabled(!m_table->selectedItems().isEmpty());
    });
}

void reservation_widget::loadMockData()
{
    QDate today = QDate::currentDate();
    m_items.clear();

    m_items.append({"C++ Primer", today.addDays(-2).toString("yyyy-MM-dd"), "排队中", 1});
    m_items.append({"设计模式", today.addDays(-1).toString("yyyy-MM-dd"), "可取", 0});
    m_items.append({"算法导论", today.addDays(-5).toString("yyyy-MM-dd"), "已过期", 0});
    m_items.append({"编译原理", today.addDays(-1).toString("yyyy-MM-dd"), "排队中", 3});
    m_items.append({"深入理解计算机系统", today.addDays(-3).toString("yyyy-MM-dd"), "可取", 0});

    updateTable();
}

void reservation_widget::updateTable()
{
    m_table->setRowCount(0);
    for (const auto &item : m_items) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(item.title));
        m_table->setItem(row, 1, new QTableWidgetItem(item.reserveDate));

        // 状态列（带颜色）
        QTableWidgetItem *statusItem = new QTableWidgetItem(item.status);
        if (item.status == "排队中") {
            statusItem->setForeground(QColor("#1976D2"));   // 蓝色
        } else if (item.status == "可取") {
            statusItem->setForeground(QColor("#388e3c"));   // 绿色
        } else if (item.status == "已过期") {
            statusItem->setForeground(QColor("#d32f2f"));   // 红色
        }
        m_table->setItem(row, 2, statusItem);

        // 队列位置（排队中才显示数字，否则显示 "-"）
        QString queueStr = (item.status == "排队中") ? QString::number(item.queuePosition) : "-";
        m_table->setItem(row, 3, new QTableWidgetItem(queueStr));
    }
    m_statusLabel->setText(QString("共 %1 条预约记录").arg(m_items.size()));
}

void reservation_widget::onCancelReservation()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    ReservationItem item = m_items.at(row);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "取消预约",
        QString("确定取消对《%1》的预约吗？").arg(item.title),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_items.removeAt(row);
        updateTable();
        m_statusLabel->setText("预约已取消");
    }
}

void reservation_widget::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
    // 未来接入真实数据
}