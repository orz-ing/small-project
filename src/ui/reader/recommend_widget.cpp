#include "recommend_widget.h"
#include "bridge/api_bridge.h"
#include "ui/book_tooltip.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>

RecommendWidget::RecommendWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void RecommendWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("⭐ 智能推荐");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* infoLabel = new QLabel("基于您和相似读者的借阅记录，为您推荐以下图书");
    infoLabel->setStyleSheet("color: #7f8c8d;");
    layout->addWidget(infoLabel);

    auto* refreshBtn = new QPushButton("🔄 刷新推荐");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &RecommendWidget::refreshRecommendations);
    layout->addWidget(refreshBtn);

    m_infoLabel = new QLabel;
    m_infoLabel->setStyleSheet("color: #7f8c8d; padding: 5px 0;");
    layout->addWidget(m_infoLabel);

    m_table = new QTableWidget;
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "书名", "作者", "出版社", "操作"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_table, 1);

    refreshRecommendations();

    // 安装书名悬停预览
    installBookTitleHover(m_table, 1, 0);
}

void RecommendWidget::refreshRecommendations() {
    int userId = ApiBridge::instance()->currentUser().id;
    auto books = ApiBridge::instance()->getRecommendations(userId, 10);

    m_table->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        auto& b = books[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(b.title));
        m_table->setItem(i, 2, new QTableWidgetItem(b.author));
        m_table->setItem(i, 3, new QTableWidgetItem(b.publisher));

        auto* btnWidget = new QWidget;
        auto* btnLayout = new QHBoxLayout(btnWidget);
        btnLayout->setContentsMargins(2, 2, 2, 2);
        btnLayout->setSpacing(4);

        int bookId = b.id;
        if (b.availableStock > 0) {
            auto* borrowBtn = new QPushButton("借阅");
            borrowBtn->setObjectName("successBtn");
            connect(borrowBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->borrowBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); refreshRecommendations(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(borrowBtn);
        } else {
            auto* reserveBtn = new QPushButton("预约");
            reserveBtn->setObjectName("warningBtn");
            connect(reserveBtn, &QPushButton::clicked, this, [this, bookId]() {
                auto user = ApiBridge::instance()->currentUser();
                Result r = ApiBridge::instance()->reserveBook(user.id, bookId);
                if (r.success) { QMessageBox::information(this, "成功", r.message); refreshRecommendations(); }
                else { QMessageBox::warning(this, "失败", r.message); }
            });
            btnLayout->addWidget(reserveBtn);
        }
        m_table->setCellWidget(i, 4, btnWidget);
    }
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_infoLabel->setText(QString("为您推荐 %1 本书").arg(books.size()));
}

