#include "search_widget.h"
#include <QVBoxLayout>
#include <QHeaderView>
search_widget::search_widget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadMockData();
}

void search_widget::setupUI()
{
    QVBoxLayout *lay = new QVBoxLayout(this);

    // 搜索栏
    QHBoxLayout *searchLay = new QHBoxLayout;
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("输入书名、作者或关键词...");
    m_searchBtn = new QPushButton("搜 索");
    searchLay->addWidget(m_searchEdit, 1);
    searchLay->addWidget(m_searchBtn);
    lay->addLayout(searchLay);

    // 状态标签
    m_statusLabel = new QLabel("共 0 条结果");
    m_statusLabel->setStyleSheet("color: #666; padding: 2px;");
    lay->addWidget(m_statusLabel);

    // 结果表格
    m_table = new QTableWidget;
    m_table->setStyleSheet(
        "QTableWidget { color: #333333; background-color: white; }"
        "QTableWidget::item { color: #333333; }"
        );
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"书名", "作者", "ISBN", "库存"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    lay->addWidget(m_table);

    connect(m_searchBtn, &QPushButton::clicked, this, &search_widget::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &search_widget::onSearch);
}

void search_widget::loadMockData()
{
    // 先造20本假书，字段顺序与表格列一致
    m_mockBooks.clear();
    m_mockBooks.append({"C++ Primer", "Stanley Lippman", "978-0321714114", "5"});
    m_mockBooks.append({"Effective C++", "Scott Meyers", "978-0321334879", "3"});
    m_mockBooks.append({"设计模式", "Erich Gamma", "978-0201633610", "2"});
    m_mockBooks.append({"算法导论", "Thomas Cormen", "978-0262033848", "4"});
    m_mockBooks.append({"编译原理", "Alfred Aho", "978-0201100884", "1"});
    m_mockBooks.append({"深入理解计算机系统", "Randal Bryant", "978-0134092669", "6"});
    m_mockBooks.append({"JavaScript高级程序设计", "Matt Frisbie", "978-1119366447", "3"});
    m_mockBooks.append({"Python编程：从入门到实践", "Eric Matthes", "978-1593279288", "7"});
    // ... 可以继续加，先造这些
}

void search_widget::onSearch()
{
    QString keyword = m_searchEdit->text().trimmed();
    updateTable(keyword);
}

void search_widget::updateTable(const QString &keyword)
{
    m_table->setRowCount(0);

    if (keyword.isEmpty()) {
        // 如果搜索框为空，显示全部假数据
        for (const auto &book : m_mockBooks) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(book[0]));
            m_table->setItem(row, 1, new QTableWidgetItem(book[1]));
            m_table->setItem(row, 2, new QTableWidgetItem(book[2]));
            m_table->setItem(row, 3, new QTableWidgetItem(book[3]));
        }
        m_statusLabel->setText(QString("共 %1 条结果").arg(m_mockBooks.size()));
        return;
    }

    // 简单过滤：书名或作者包含关键字
    int count = 0;
    for (const auto &book : m_mockBooks) {
        if (book[0].contains(keyword, Qt::CaseInsensitive) ||
            book[1].contains(keyword, Qt::CaseInsensitive)) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setItem(row, 0, new QTableWidgetItem(book[0]));
            m_table->setItem(row, 1, new QTableWidgetItem(book[1]));
            m_table->setItem(row, 2, new QTableWidgetItem(book[2]));
            m_table->setItem(row, 3, new QTableWidgetItem(book[3]));
            ++count;
        }
    }
    m_statusLabel->setText(QString("找到 %1 条结果").arg(count));
}