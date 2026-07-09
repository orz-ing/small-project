#include "search_widget.h"
#include "bridge/api_bridge.h"
#include "ui/book_tooltip.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QHBoxLayout>

SearchWidget::SearchWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void SearchWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📖 搜索图书");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* searchLayout = new QHBoxLayout;
    m_searchInput = new QLineEdit;
    m_searchInput->setObjectName("searchBox");
    m_searchInput->setPlaceholderText("输入书名、作者、ISBN、关键词...");
    m_searchBtn = new QPushButton("🔍 搜索");
    m_searchBtn->setObjectName("primaryBtn");
    searchLayout->addWidget(m_searchInput, 1);
    searchLayout->addWidget(m_searchBtn);
    layout->addLayout(searchLayout);

    m_resultCountLabel = new QLabel;
    m_resultCountLabel->setStyleSheet("color: #7f8c8d; font-size: 13px;");
    layout->addWidget(m_resultCountLabel);

    m_resultTable = new QTableWidget;
    m_resultTable->setColumnCount(7);
    m_resultTable->setHorizontalHeaderLabels({"ID", "书名", "作者", "ISBN", "出版社", "库存", "操作"});
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->setColumnHidden(0, true);
    m_resultTable->verticalHeader()->hide();
    m_resultTable->setAlternatingRowColors(true);
    layout->addWidget(m_resultTable, 1);

    connect(m_searchBtn, &QPushButton::clicked, this, &SearchWidget::onSearch);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &SearchWidget::onSearch);

    // 安装书名悬停预览
    installBookTitleHover(m_resultTable, 1, 0);
}

void SearchWidget::onSearch() {
    QString keywords = m_searchInput->text().trimmed();
    if (keywords.isEmpty()) return;

    auto results = ApiBridge::instance()->searchBooks(keywords);
    m_resultCountLabel->setText(QString("找到 %1 条结果").arg(results.size()));
    m_resultTable->setRowCount(results.size());

    for (int i = 0; i < results.size(); ++i) {
        auto& b = results[i];
        m_resultTable->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        m_resultTable->setItem(i, 1, new QTableWidgetItem(b.title));
        m_resultTable->setItem(i, 2, new QTableWidgetItem(b.author));
        m_resultTable->setItem(i, 3, new QTableWidgetItem(b.isbn));
        m_resultTable->setItem(i, 4, new QTableWidgetItem(b.publisher));
        m_resultTable->setItem(i, 5, new QTableWidgetItem(QString::number(b.availableStock) + "/" + QString::number(b.totalStock)));

        auto* btnWidget = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(2, 2, 2, 2);
        btnLayout->setSpacing(4);

        int bookId = b.id;
        int stock = b.availableStock;
        if (stock > 0) {
            auto* borrowBtn = new QPushButton("借阅");
            borrowBtn->setObjectName("successBtn");
            connect(borrowBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->borrowBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); onSearch(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(borrowBtn);
        } else {
            auto* reserveBtn = new QPushButton("预约");
            reserveBtn->setObjectName("warningBtn");
            connect(reserveBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->reserveBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); onSearch(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(reserveBtn);
        }
        m_resultTable->setCellWidget(i, 6, btnWidget);
    }
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

