#include "recommend_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

recommend_widget::recommend_widget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void recommend_widget::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(6);

    m_statusLabel = new QLabel("根据你的阅读偏好，为你推荐以下图书");
    m_statusLabel->setStyleSheet("color: #666; padding: 0px; margin-left: 2px;");
    lay->addWidget(m_statusLabel);

    // 表格
    m_table = new QTableWidget;
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"书名", "作者", "推荐理由"});

    // 保持文字可见
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        "QTableWidget::item { color: #333333; }"
        );

    QHeaderView *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);  // 书名拉伸
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->resizeSection(1, 120);   // 作者列固定宽度

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    // 按钮区
    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addStretch();
    m_borrowBtn = new QPushButton("借阅");
    m_borrowBtn->setEnabled(false);
    btnLay->addWidget(m_borrowBtn);
    lay->addLayout(btnLay);

    connect(m_borrowBtn, &QPushButton::clicked, this, &recommend_widget::onBorrow);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        m_borrowBtn->setEnabled(!m_table->selectedItems().isEmpty());
    });
}

void recommend_widget::loadMockData()
{
    m_items.clear();

    m_items.append({"Effective Modern C++", "Scott Meyers", "借过《C++ Primer》的用户也喜欢"});
    m_items.append({"重构：改善既有代码的设计", "Martin Fowler", "你的借阅历史中包含大量设计类书籍"});
    m_items.append({"操作系统概念", "Abraham Silberschatz", "与你借阅的《深入理解计算机系统》类似"});
    m_items.append({"计算机网络：自顶向下方法", "James Kurose", "根据你的专业方向推荐"});
    m_items.append({"数据库系统概念", "Abraham Silberschatz", "借过《算法导论》的用户也喜欢"});

    updateTable();
}

void recommend_widget::updateTable()
{
    m_table->setRowCount(0);
    for (const auto &item : m_items) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(item.title));
        m_table->setItem(row, 1, new QTableWidgetItem(item.author));

        // 推荐理由用灰色斜体（通过 QTableWidgetItem 的 font 属性）
        QTableWidgetItem *reasonItem = new QTableWidgetItem(item.reason);
        QFont font = reasonItem->font();
        font.setItalic(true);
        reasonItem->setFont(font);
        reasonItem->setForeground(QColor("#888888"));
        m_table->setItem(row, 2, reasonItem);
    }
    m_statusLabel->setText(QString("为你推荐 %1 本书").arg(m_items.size()));
}

void recommend_widget::onBorrow()
{
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    RecommendItem item = m_items.at(row);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "借阅推荐图书",
        QString("确定要借阅《%1》吗？").arg(item.title),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 模拟借阅成功，从推荐列表中移除该书（现实中不会移除，这里仅为演示）
        m_items.removeAt(row);
        updateTable();
        m_statusLabel->setText(QString("借阅《%1》成功！").arg(item.title));
    }
}

void recommend_widget::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
}