#include "book_manager_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include "main_window.h"
BookManagerWidget::BookManagerWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void BookManagerWidget::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);

    // 搜索栏
    QHBoxLayout *searchLay = new QHBoxLayout;
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("输入ISBN或书名搜索...");
    QPushButton *searchBtn = new QPushButton("搜索");
    searchLay->addWidget(m_searchEdit, 1);
    searchLay->addWidget(searchBtn);
    lay->addLayout(searchLay);

    m_statusLabel = new QLabel("共 0 本图书");
    m_statusLabel->setStyleSheet("color: #666; padding: 2px;");
    lay->addWidget(m_statusLabel);

    // 表格
    m_table = new QTableWidget;
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"ISBN", "书名", "作者", "库存"});
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        "QTableWidget::item { color: #333333; }"
        );
    QHeaderView *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->resizeSection(0, 130);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->resizeSection(3, 60);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    // 按钮区
    QHBoxLayout *btnLay = new QHBoxLayout;
    btnLay->addStretch();
    m_addBtn = new QPushButton("添加");
    m_editBtn = new QPushButton("编辑");
    m_deleteBtn = new QPushButton("删除");
    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
    btnLay->addWidget(m_addBtn);
    btnLay->addWidget(m_editBtn);
    btnLay->addWidget(m_deleteBtn);
    lay->addLayout(btnLay);

    connect(searchBtn, &QPushButton::clicked, this, &BookManagerWidget::onSearch);
    connect(m_addBtn, &QPushButton::clicked, this, &BookManagerWidget::onAddBook);
    connect(m_editBtn, &QPushButton::clicked, this, &BookManagerWidget::onEditBook);
    connect(m_deleteBtn, &QPushButton::clicked, this, &BookManagerWidget::onDeleteBook);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool sel = !m_table->selectedItems().isEmpty();
        m_editBtn->setEnabled(sel);
        m_deleteBtn->setEnabled(sel);
    });
}

void BookManagerWidget::loadMockData()
{
    m_books.clear();
    m_books.append({"978-0321714114", "C++ Primer", "Stanley Lippman", 5});
    m_books.append({"978-0321334879", "Effective Modern C++", "Scott Meyers", 3});
    m_books.append({"978-0201633610", "设计模式", "Erich Gamma", 2});
    m_books.append({"978-0262033848", "算法导论", "Thomas Cormen", 4});
    m_books.append({"978-0134092669", "深入理解计算机系统", "Randal Bryant", 6});
    updateTable();
}

void BookManagerWidget::updateTable()
{
    m_table->setRowCount(0);
    for (const auto &book : m_books) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(book.isbn));
        m_table->setItem(row, 1, new QTableWidgetItem(book.title));
        m_table->setItem(row, 2, new QTableWidgetItem(book.author));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(book.stock)));
    }
    m_statusLabel->setText(QString("共 %1 本图书").arg(m_books.size()));
}

void BookManagerWidget::onSearch()
{
    QString keyword = m_searchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        updateTable();
        return;
    }
    m_table->setRowCount(0);
    for (const auto &book : m_books) {
        if (book.isbn.contains(keyword, Qt::CaseInsensitive) ||
            book.title.contains(keyword, Qt::CaseInsensitive)) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(book.isbn));
            m_table->setItem(row, 1, new QTableWidgetItem(book.title));
            m_table->setItem(row, 2, new QTableWidgetItem(book.author));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(book.stock)));
        }
    }
    m_statusLabel->setText("搜索结果");
}

void BookManagerWidget::onAddBook()
{
    QString isbn = QInputDialog::getText(this, "添加图书", "ISBN:");
    QString title = QInputDialog::getText(this, "添加图书", "书名:");
    QString author = QInputDialog::getText(this, "添加图书", "作者:");
    int stock = QInputDialog::getInt(this, "添加图书", "库存:", 1);
    if (!isbn.isEmpty() && !title.isEmpty()) {
        m_books.append({isbn, title, author, stock});
        updateTable();
    }
    MainWindow *mainWin = qobject_cast<MainWindow*>(window());
    if (mainWin) mainWin->showNotification("图书添加成功", 0);
}

void BookManagerWidget::onEditBook()
{
    int row = m_table->currentRow();
    if (row < 0) return;
    int newStock = QInputDialog::getInt(this, "编辑图书", "新库存:", m_books[row].stock);
    m_books[row].stock = newStock;
    updateTable();

}

void BookManagerWidget::onDeleteBook()
{
    int row = m_table->currentRow();
    if (row < 0) return;
    if (QMessageBox::Yes == QMessageBox::question(this, "删除图书", "确定删除此书？")) {
        m_books.removeAt(row);
        updateTable();
    }
    MainWindow *mainWin = qobject_cast<MainWindow*>(window());
    if (mainWin) mainWin->showNotification("图书已删除", 0);
}