#include "borrow_panel.h"
#include "bridge/api_bridge.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>

BorrowPanel::BorrowPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
    refreshData();
}

void BorrowPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📚 我的借阅");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* refreshBtn = new QPushButton("🔄 刷新");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &BorrowPanel::refreshData);
    layout->addWidget(refreshBtn);

    m_summaryLabel = new QLabel;
    m_summaryLabel->setStyleSheet("color: #7f8c8d; padding: 5px 0;");
    layout->addWidget(m_summaryLabel);

    m_tabWidget = new QTabWidget;

    // 当前借阅
    m_activeTable = new QTableWidget;
    m_activeTable->setColumnCount(6);
    m_activeTable->setHorizontalHeaderLabels({"记录ID", "书名", "借阅日期", "应还日期", "状态", "操作"});
    m_activeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_activeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_activeTable->setColumnHidden(0, true);
    m_activeTable->verticalHeader()->hide();
    m_activeTable->horizontalHeader()->setStretchLastSection(true);
    m_activeTable->verticalHeader()->setDefaultSectionSize(50);
    m_tabWidget->addTab(m_activeTable, "当前借阅");

    // 历史记录
    m_historyTable = new QTableWidget;
    m_historyTable->setColumnCount(7);
    m_historyTable->setHorizontalHeaderLabels({"记录ID", "书名", "借阅日期", "归还日期", "罚款", "状态", "操作"});
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setColumnHidden(0, true);
    m_historyTable->verticalHeader()->hide();
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->verticalHeader()->setDefaultSectionSize(50);
    m_tabWidget->addTab(m_historyTable, "历史记录");

    layout->addWidget(m_tabWidget, 1);
}

void BorrowPanel::refreshData() {
    int userId = ApiBridge::instance()->currentUser().id;

    // 当前借阅
    auto activeRecords = ApiBridge::instance()->getMyActiveRecords(userId);
    m_activeTable->setRowCount(activeRecords.size());

    for (int i = 0; i < activeRecords.size(); ++i) {
        auto& r = activeRecords[i];
        m_activeTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        m_activeTable->setItem(i, 1, new QTableWidgetItem(r.bookTitle));
        m_activeTable->setItem(i, 2, new QTableWidgetItem(r.borrowDate.toString("yyyy-MM-dd")));
        m_activeTable->setItem(i, 3, new QTableWidgetItem(r.dueDate.toString("yyyy-MM-dd")));

        bool overdue = r.dueDate < QDateTime::currentDateTime();
        QString status = overdue ? "⚠️ 已逾期" : "📖 借阅中";
        m_activeTable->setItem(i, 4, new QTableWidgetItem(status));
        if (overdue) {
            m_activeTable->item(i, 4)->setForeground(Qt::red);
        }

        auto* btnLayout = new QWidget;
        auto* btnWidget = new QHBoxLayout(btnLayout);
        btnWidget->setContentsMargins(10, 8, 10, 8);
        btnWidget->setSpacing(10);

        auto* returnBtn = new QPushButton("归还");
        returnBtn->setObjectName("warningBtn");
        returnBtn->setStyleSheet("font-size:12px;padding:5px 10px");
        int recordId = r.id;
        connect(returnBtn, &QPushButton::clicked, this, [this, recordId]() { onReturnBook(recordId); });
        btnWidget->addWidget(returnBtn);

        auto* renewBtn = new QPushButton("续借");
        renewBtn->setObjectName("primaryBtn");
        renewBtn->setStyleSheet("font-size:12px;padding:5px 10px");
        connect(renewBtn, &QPushButton::clicked, this, [this, recordId]() { onRenewBook(recordId); });
        btnWidget->addWidget(renewBtn);

        m_activeTable->setCellWidget(i, 5, btnLayout);
    }
    m_activeTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 历史记录
    auto allRecords = ApiBridge::instance()->getMyBorrowRecords(userId);
    QVector<BorrowRecord> history;
    for (const auto& r : allRecords) {
        if (r.status == BorrowStatus::Returned) history.append(r);
    }
    m_historyTable->setRowCount(history.size());

    for (int i = 0; i < history.size(); ++i) {
        auto& r = history[i];
        m_historyTable->setItem(i, 0, new QTableWidgetItem(QString::number(r.id)));
        m_historyTable->setItem(i, 1, new QTableWidgetItem(r.bookTitle));
        m_historyTable->setItem(i, 2, new QTableWidgetItem(r.borrowDate.toString("yyyy-MM-dd")));
        m_historyTable->setItem(i, 3, new QTableWidgetItem(r.returnDate.toString("yyyy-MM-dd")));
        m_historyTable->setItem(i, 4, new QTableWidgetItem(QString::number(r.fine, 'f', 1) + " 元"));
        m_historyTable->setItem(i, 5, new QTableWidgetItem("✅ 已归还"));
    }
    m_historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    m_summaryLabel->setText(QString("当前借阅 %1 本，历史记录 %2 条")
        .arg(activeRecords.size()).arg(history.size()));
}

void BorrowPanel::onReturnBook(int recordId) {
    Result r = ApiBridge::instance()->returnBook(recordId);
    if (r.success) {
        QMessageBox::information(this, "归还成功", r.message);
        refreshData();
    } else {
        QMessageBox::warning(this, "归还失败", r.message);
    }
}

void BorrowPanel::onRenewBook(int recordId) {
    Result r = ApiBridge::instance()->renewBook(recordId);
    if (r.success) {
        QMessageBox::information(this, "续借成功", r.message);
        refreshData();
    } else {
        QMessageBox::warning(this, "续借失败", r.message);
    }
}


