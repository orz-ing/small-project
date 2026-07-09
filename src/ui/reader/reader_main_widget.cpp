#include "reader_main_widget.h"
#include "bridge/api_bridge.h"
#include "ui/book_tooltip.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QHBoxLayout>

ReaderMainWidget::ReaderMainWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ReaderMainWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto user = ApiBridge::instance()->currentUser();
    m_welcomeLabel = new QLabel("欢迎回来，" + user.displayName);
    m_welcomeLabel->setObjectName("sectionTitle");
    layout->addWidget(m_welcomeLabel);

    // 搜索栏 + 浏览全部按钮
    auto* searchLayout = new QHBoxLayout;
    m_searchInput = new QLineEdit;
    m_searchInput->setObjectName("searchBox");
    m_searchInput->setPlaceholderText("输入书名、作者、ISBN 搜索...");
    m_searchBtn = new QPushButton("🔍 搜索");
    m_searchBtn->setObjectName("primaryBtn");
    searchLayout->addWidget(m_searchInput, 1);
    searchLayout->addWidget(m_searchBtn);

    auto* allBooksBtn = new QPushButton("📖 浏览全部");
    allBooksBtn->setObjectName("successBtn");
    connect(allBooksBtn, &QPushButton::clicked, this, &ReaderMainWidget::loadAllBooks);
    searchLayout->addWidget(allBooksBtn);

    layout->addLayout(searchLayout);

    connect(m_searchBtn, &QPushButton::clicked, this, &ReaderMainWidget::onSearch);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &ReaderMainWidget::onSearch);

    // 搜索结果
    auto* resultGroup = new QGroupBox("图书列表");
    auto* resultLayout = new QVBoxLayout(resultGroup);
    m_resultTable = new QTableWidget;
    m_resultTable->setColumnCount(6);
    m_resultTable->setHorizontalHeaderLabels({"ID", "书名", "作者", "ISBN", "库存", "操作"});
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    m_resultTable->setColumnHidden(0, true);
    m_resultTable->horizontalHeader()->setStretchLastSection(false);
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_resultTable->setColumnWidth(2, 120);
    m_resultTable->setColumnWidth(3, 140);
    m_resultTable->setColumnWidth(4, 70);
    m_resultTable->setColumnWidth(5, 160);
    m_resultTable->verticalHeader()->hide();
    m_resultTable->verticalHeader()->setDefaultSectionSize(50);
    m_resultTable->setColumnWidth(1, 220);
    m_resultTable->setColumnWidth(2, 150);
    m_resultTable->setColumnWidth(3, 150);
    connect(m_resultTable, &QTableWidget::cellDoubleClicked, this, &ReaderMainWidget::onBookDoubleClicked);
    resultLayout->addWidget(m_resultTable);
    layout->addWidget(resultGroup, 2);

    // 热门推荐
    auto* hotGroup = new QGroupBox("热门推荐");
    auto* hotLayout = new QVBoxLayout(hotGroup);
    m_hotTable = new QTableWidget;
    m_hotTable->setColumnCount(4);
    m_hotTable->setHorizontalHeaderLabels({"书名", "作者", "借阅次数", "操作"});
    m_hotTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_hotTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_hotTable->horizontalHeader()->setStretchLastSection(true);
    m_hotTable->verticalHeader()->hide();
    m_hotTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_hotTable->setColumnWidth(1, 120);
    m_hotTable->setColumnWidth(2, 90);
    m_hotTable->verticalHeader()->hide();
        m_hotTable->verticalHeader()->setDefaultSectionSize(50);
    hotLayout->addWidget(m_hotTable);
    layout->addWidget(hotGroup, 1);

    loadHotBooks();
    loadAllBooks();  // 默认显示全部书籍

    // 安装书名悬停预览
    installBookTitleHover(m_resultTable, 1, 0);
    installBookTitleHover(m_hotTable, 0, -1);
}

void ReaderMainWidget::loadAllBooks() {
    auto allBooks = ApiBridge::instance()->getAllBooks();
    m_resultTable->setRowCount(allBooks.size());
    for (int i = 0; i < allBooks.size(); ++i) {
        auto& b = allBooks[i];
        m_resultTable->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        m_resultTable->setItem(i, 1, new QTableWidgetItem(b.title));
        m_resultTable->setItem(i, 2, new QTableWidgetItem(b.author));
        m_resultTable->setItem(i, 3, new QTableWidgetItem(b.isbn));
        m_resultTable->setItem(i, 4, new QTableWidgetItem(QString::number(b.availableStock) + "/" + QString::number(b.totalStock)));

        auto* btnWidget = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(10, 8, 10, 8);
        btnLayout->setSpacing(10);

        int bookId = b.id;
        int stock = b.availableStock;
        if (stock > 0) {
            auto* borrowBtn = new QPushButton("借阅");
            borrowBtn->setStyleSheet("font-size:15px;padding:6px 16px");
 borrowBtn->setObjectName("successBtn");
            connect(borrowBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->borrowBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); loadAllBooks(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(borrowBtn);
        } else {
            auto* reserveBtn = new QPushButton("预约");
            reserveBtn->setStyleSheet("font-size:15px;padding:6px 16px");
 reserveBtn->setObjectName("warningBtn");
            connect(reserveBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->reserveBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); loadAllBooks(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(reserveBtn);
        }
        m_resultTable->setCellWidget(i, 5, btnWidget);
    }
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void ReaderMainWidget::loadHotBooks() {
    auto hotBooks = ApiBridge::instance()->getHotBooks(10);
    m_hotTable->setRowCount(hotBooks.size());
    for (int i = 0; i < hotBooks.size(); ++i) {
        auto* titleItem = new QTableWidgetItem(hotBooks[i].title);
        titleItem->setData(Qt::UserRole, hotBooks[i].id);  // 存储 bookId 供悬停预览使用
        m_hotTable->setItem(i, 0, titleItem);
        m_hotTable->setItem(i, 1, new QTableWidgetItem(hotBooks[i].author));

        auto* btnWidget = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(10, 8, 10, 8);
        btnLayout->setSpacing(10);

        int bookId = hotBooks[i].id;
        if (hotBooks[i].availableStock > 0) {
            auto* borrowBtn = new QPushButton("借阅");
            borrowBtn->setStyleSheet("font-size:15px;padding:6px 16px");
 borrowBtn->setObjectName("successBtn");
            connect(borrowBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->borrowBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); loadHotBooks(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(borrowBtn);
        } else {
            auto* reserveBtn = new QPushButton("预约");
            reserveBtn->setStyleSheet("font-size:15px;padding:6px 16px");
 reserveBtn->setObjectName("warningBtn");
            connect(reserveBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->reserveBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); loadHotBooks(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(reserveBtn);
        }
        m_hotTable->setCellWidget(i, 3, btnWidget);
    }
    m_hotTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ReaderMainWidget::onSearch() {
    QString keywords = m_searchInput->text().trimmed();
    if (keywords.isEmpty()) { loadAllBooks(); return; }

    auto results = ApiBridge::instance()->searchBooks(keywords);
    m_resultTable->setRowCount(results.size());
    for (int i = 0; i < results.size(); ++i) {
        m_resultTable->setItem(i, 0, new QTableWidgetItem(QString::number(results[i].id)));
        m_resultTable->setItem(i, 1, new QTableWidgetItem(results[i].title));
        m_resultTable->setItem(i, 2, new QTableWidgetItem(results[i].author));
        m_resultTable->setItem(i, 3, new QTableWidgetItem(results[i].isbn));
        m_resultTable->setItem(i, 4, new QTableWidgetItem(QString::number(results[i].availableStock) + "/" + QString::number(results[i].totalStock)));

        auto* btnWidget = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(10, 8, 10, 8);
        btnLayout->setSpacing(10);

        int bookId = results[i].id;
        int stock = results[i].availableStock;
        if (stock > 0) {
            auto* borrowBtn = new QPushButton("借阅");
            borrowBtn->setStyleSheet("font-size:15px;padding:6px 16px");
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
            reserveBtn->setStyleSheet("font-size:15px;padding:6px 16px");
 reserveBtn->setObjectName("warningBtn");
            connect(reserveBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->reserveBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); onSearch(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(reserveBtn);
        }
        m_resultTable->setCellWidget(i, 5, btnWidget);
    }
    m_resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void ReaderMainWidget::onBookDoubleClicked(int row, int /*col*/) {
    auto item = m_resultTable->item(row, 0);
    if (item) m_selectedBookId = item->text().toInt();
}

void ReaderMainWidget::onBorrowBook() {
    if (m_selectedBookId <= 0) return;
    auto user = ApiBridge::instance()->currentUser();
    Result r = ApiBridge::instance()->borrowBook(user.id, m_selectedBookId);
    if (r.success) QMessageBox::information(this, "成功", r.message);
    else QMessageBox::warning(this, "失败", r.message);
}










