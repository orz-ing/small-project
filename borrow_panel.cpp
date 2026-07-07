#include "borrow_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>

borrow_panel::borrow_panel(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void borrow_panel::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(6);

    m_statusLabel = new QLabel("共 0 条借阅记录");
    m_statusLabel->setStyleSheet("color: #666; padding: 0px; margin-left: 2px;");
    lay->addWidget(m_statusLabel);

    // ===== 表格（只初始化一次） =====
    // 表格
    m_table = new QTableWidget;
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        );
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"书名", "借阅日期", "应还日期", "状态", "罚款"});

    // 强制表格水平方向扩展
    m_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHeaderView *header = m_table->horizontalHeader();
    // 关键：所有列均分可用宽度，永不出现水平滚动条
    header->setSectionResizeMode(QHeaderView::Stretch);
    header->setStretchLastSection(true);  // 最后一列也参与均分

    // 设置最小宽度，防止列被挤得太小
    m_table->setMinimumWidth(450);

    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 关掉滚动条
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    // 按钮区
    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addStretch();
    m_returnBtn = new QPushButton("归还");
    m_returnBtn->setEnabled(false);
    m_renewBtn = new QPushButton("续借");
    m_renewBtn->setEnabled(false);
    btnLay->addWidget(m_returnBtn);
    btnLay->addWidget(m_renewBtn);
    lay->addLayout(btnLay);

    connect(m_returnBtn, &QPushButton::clicked, this, &borrow_panel::onReturnBook);
    connect(m_renewBtn, &QPushButton::clicked, this, &borrow_panel::onRenewBook);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !m_table->selectedItems().isEmpty();
        m_returnBtn->setEnabled(hasSelection);
        m_renewBtn->setEnabled(hasSelection);
    });
}

void borrow_panel::loadMockData()
{
    QDate today = QDate::currentDate();
    m_items.clear();

    m_items.append({"C++ Primer", today.addDays(-15).toString("yyyy-MM-dd"),
                    today.addDays(15).toString("yyyy-MM-dd"), "正常", 0.0});
    m_items.append({"设计模式", today.addDays(-30).toString("yyyy-MM-dd"),
                    today.addDays(-2).toString("yyyy-MM-dd"), "已逾期", 1.0});
    m_items.append({"算法导论", today.addDays(-10).toString("yyyy-MM-dd"),
                    today.addDays(5).toString("yyyy-MM-dd"), "正常", 0.0});
    m_items.append({"编译原理", today.addDays(-20).toString("yyyy-MM-dd"),
                    today.addDays(2).toString("yyyy-MM-dd"), "即将到期", 0.0});
    m_items.append({"深入理解计算机系统", today.addDays(-7).toString("yyyy-MM-dd"),
                    today.addDays(23).toString("yyyy-MM-dd"), "正常", 0.0});

    updateTable();
}

void borrow_panel::updateTable()
{
    m_table->setRowCount(0);
    for (const auto &item : m_items) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(item.title));
        m_table->setItem(row, 1, new QTableWidgetItem(item.borrowDate));
        m_table->setItem(row, 2, new QTableWidgetItem(item.dueDate));

        QTableWidgetItem *statusItem = new QTableWidgetItem(item.status);
        if (item.status == "已逾期") {
            statusItem->setForeground(QColor("#d32f2f"));
        } else if (item.status == "即将到期") {
            statusItem->setForeground(QColor("#f57c00"));
        } else {
            statusItem->setForeground(QColor("#388e3c"));
        }
        m_table->setItem(row, 3, statusItem);

        QString fineStr = item.fine > 0 ? QString::number(item.fine, 'f', 2) : "0.00";
        m_table->setItem(row, 4, new QTableWidgetItem(fineStr));
    }
    m_statusLabel->setText(QString("共 %1 条借阅记录").arg(m_items.size()));
}

void borrow_panel::onReturnBook()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    BorrowItem item = m_items.at(row);
    QString msg;
    if (item.fine > 0) {
        msg = QString("本书已逾期，需缴纳罚款 %1 元。是否确认归还？").arg(item.fine, 0, 'f', 2);
    } else {
        msg = "确认归还？";
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "归还图书", msg,
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_items.removeAt(row);
        updateTable();
        m_statusLabel->setText("归还成功！" + (item.fine > 0 ? QString("已记录罚款 %1 元。").arg(item.fine) : ""));
    }
}

void borrow_panel::onRenewBook()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    BorrowItem &item = m_items[row];
    if (item.status == "已逾期") {
        QMessageBox::warning(this, "续借失败", "逾期图书无法续借，请先归还。");
        return;
    }

    QDate currentDue = QDate::fromString(item.dueDate, "yyyy-MM-dd");
    QDate newDue = currentDue.addDays(30);
    item.dueDate = newDue.toString("yyyy-MM-dd");

    QDate today = QDate::currentDate();
    if (newDue < today) {
        item.status = "已逾期";
    } else if (newDue <= today.addDays(3)) {
        item.status = "即将到期";
    } else {
        item.status = "正常";
    }

    updateTable();
    m_statusLabel->setText(QString("续借成功，新的应还日期：%1").arg(item.dueDate));
}

void borrow_panel::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
}