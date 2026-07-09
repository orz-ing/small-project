#include "book_manager_widget.h"
#include "bridge/api_bridge.h"
#include "ui/book_tooltip.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QTextEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSet>
#include <QHash>
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

BookManagerWidget::BookManagerWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void BookManagerWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📖 图书管理");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* splitter = new QSplitter(Qt::Horizontal);

    auto* leftWidget = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto* catLabel = new QLabel("📂 分类（悬浮查看子分类）");
    catLabel->setStyleSheet("font-weight: bold; font-size: 15px;");
    leftLayout->addWidget(catLabel);

    m_categoryTree = new QTreeWidget;
    m_categoryTree->setHeaderLabel("分类名称");
    m_categoryTree->setMinimumWidth(180);
    m_categoryTree->setMouseTracking(true);
    connect(m_categoryTree, &QTreeWidget::itemClicked, this, &BookManagerWidget::onCategorySelected);
    connect(m_categoryTree, &QTreeWidget::itemEntered, this, &BookManagerWidget::onTreeItemEntered);
    m_categoryTree->installEventFilter(this);
    leftLayout->addWidget(m_categoryTree);

    // 创建子分类悬浮弹出面板
    m_subcatPopup = new QFrame(this);
    m_subcatPopup->setObjectName("subcatPopup");
    m_subcatPopup->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_subcatPopup->setAttribute(Qt::WA_ShowWithoutActivating);
    m_subcatPopup->setAttribute(Qt::WA_Hover);
    m_subcatPopup->installEventFilter(this);
    m_subcatLayout = new QVBoxLayout(m_subcatPopup);
    m_subcatLayout->setContentsMargins(8, 8, 8, 8);
    m_subcatLayout->setSpacing(4);

    auto* rightWidget = new QWidget;
    auto* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    auto* toolLayout = new QHBoxLayout;
    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText("搜索图书...");
    toolLayout->addWidget(m_searchInput, 1);

    m_addBtn = new QPushButton("➕ 添加");
    m_addBtn->setObjectName("successBtn");
    m_editBtn = new QPushButton("✏️ 编辑");
    m_editBtn->setObjectName("primaryBtn");
    m_deleteBtn = new QPushButton("🗑️ 删除");
    m_deleteBtn->setObjectName("dangerBtn");
    toolLayout->addWidget(m_addBtn);
    toolLayout->addWidget(m_editBtn);
    toolLayout->addWidget(m_deleteBtn);
    rightLayout->addLayout(toolLayout);

    m_bookTable = new QTableWidget;
    m_bookTable->setColumnCount(8);
    m_bookTable->setHorizontalHeaderLabels({"ID", "书名", "作者", "ISBN", "出版社", "分类", "总库存", "可用库存"});
    m_bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_bookTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_bookTable->setColumnHidden(0, true);
    m_bookTable->verticalHeader()->hide();
    m_bookTable->setAlternatingRowColors(true);
    m_bookTable->horizontalHeader()->setStretchLastSection(true);
    rightLayout->addWidget(m_bookTable, 1);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    layout->addWidget(splitter, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &BookManagerWidget::onAddBook);
    connect(m_editBtn, &QPushButton::clicked, this, &BookManagerWidget::onEditBook);
    connect(m_deleteBtn, &QPushButton::clicked, this, &BookManagerWidget::onDeleteBook);
    connect(m_searchInput, &QLineEdit::returnPressed, this, &BookManagerWidget::onSearch);

    refreshCategoryTree();
    refreshBookList();

    // 安装书名悬停预览
    installBookTitleHover(m_bookTable, 1, 0);
}

void BookManagerWidget::refreshCategoryTree() {
    m_categoryTree->clear();

    auto* allItem = new QTreeWidgetItem(m_categoryTree, {"全部"});
    allItem->setData(0, Qt::UserRole, -1);

    // 只显示根分类，子分类通过悬浮弹出查看
    auto rootCats = ApiBridge::instance()->getRootCategories();
    for (const auto& root : rootCats) {
        auto* rootItem = new QTreeWidgetItem(m_categoryTree, {root.name + QStringLiteral(" ▸")});
        rootItem->setData(0, Qt::UserRole, root.id);
    }
}

void BookManagerWidget::onCategorySelected(QTreeWidgetItem* item, int /*col*/) {
    hideSubcatPopup();
    if (!item) {
        m_currentCategoryId = -1;
    } else {
        m_currentCategoryId = item->data(0, Qt::UserRole).toInt();
    }
    refreshBookList();
}

void BookManagerWidget::onTreeItemEntered(QTreeWidgetItem* item, int /*column*/) {
    if (!item) { hideSubcatPopup(); return; }

    int catId = item->data(0, Qt::UserRole).toInt();
    if (catId == -1) { hideSubcatPopup(); return; }

    auto allCats = ApiBridge::instance()->getAllCategories();
    bool isRoot = false;
    for (const auto& c : allCats) {
        if (c.id == catId && c.parentId == -1) {
            isRoot = true;
            break;
        }
    }

    if (!isRoot) { hideSubcatPopup(); return; }

    auto children = ApiBridge::instance()->getCategoryChildren(catId);
    if (children.isEmpty()) { hideSubcatPopup(); return; }

    m_hoverRootId = catId;
    QRect itemRect = m_categoryTree->visualItemRect(item);
    QPoint globalPos = m_categoryTree->viewport()->mapToGlobal(itemRect.topRight());
    showSubcatPopup(item, globalPos);
}

void BookManagerWidget::showSubcatPopup(QTreeWidgetItem* item, const QPoint& pos) {
    QLayoutItem* child;
    while ((child = m_subcatLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    int rootId = item->data(0, Qt::UserRole).toInt();
    QString rootName = item->text(0);

    auto* titleLabel = new QLabel("📂 " + rootName);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #1a3d6b; padding: 4px 8px;");
    m_subcatLayout->addWidget(titleLabel);

    auto* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #dce1e8;");
    m_subcatLayout->addWidget(sep);

    auto children = ApiBridge::instance()->getCategoryChildren(rootId);
    for (const auto& childCat : children) {
        auto* btn = new QPushButton("  " + childCat.name);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: #f4f7fc;"
            "  border: none;"
            "  border-radius: 6px;"
            "  padding: 8px 16px;"
            "  text-align: left;"
            "  font-size: 13px;"
            "  color: #1a2332;"
            "}"
            "QPushButton:hover {"
            "  background-color: #2d6a9f;"
            "  color: white;"
            "}"
        );
        int childId = childCat.id;
        connect(btn, &QPushButton::clicked, this, [this, childId]() {
            onSubcatClicked(childId);
        });
        m_subcatLayout->addWidget(btn);
    }

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeo = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);
    int popupX = qMin(pos.x(), screenGeo.right() - 220);
    int popupY = qMin(pos.y(), screenGeo.bottom() - 300);
    if (popupY < 0) popupY = 0;
    m_subcatPopup->move(popupX, popupY);
    m_subcatPopup->adjustSize();
    m_subcatPopup->show();
    m_subcatPopup->raise();
}

void BookManagerWidget::hideSubcatPopup() {
    if (m_subcatPopup) {
        m_subcatPopup->hide();
    }
    m_hoverRootId = -1;
}

void BookManagerWidget::onSubcatClicked(int categoryId) {
    m_currentCategoryId = categoryId;
    hideSubcatPopup();
    refreshBookList();
}

bool BookManagerWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        hideSubcatPopup();
    }

    if (event->type() == QEvent::Leave) {
        if (watched == m_categoryTree || watched == m_subcatPopup) {
            QTimer::singleShot(150, this, [this]() {
                if (m_subcatPopup && m_subcatPopup->isVisible()) {
                    QRect popupRect(m_subcatPopup->pos(), m_subcatPopup->size());
                    QPoint cursor = QCursor::pos();
                    QPoint treeTL = m_categoryTree->mapToGlobal(QPoint(0, 0));
                    QRect treeRect(treeTL, m_categoryTree->size());

                    if (!popupRect.contains(cursor) && !treeRect.contains(cursor)) {
                        hideSubcatPopup();
                    }
                }
            });
        }
    }

    return QWidget::eventFilter(watched, event);
}

void BookManagerWidget::refreshBookList() {
    QVector<Book> books = ApiBridge::instance()->getAllBooks();

    if (m_currentCategoryId >= 0) {
        auto allCats = ApiBridge::instance()->getAllCategories();
        QHash<int, QVector<int>> childMap;
        for (const auto& c : allCats) {
            childMap[c.parentId].append(c.id);
        }

        QSet<int> ids;
        QVector<int> stack = {m_currentCategoryId};
        while (!stack.isEmpty()) {
            int id = stack.takeLast();
            ids.insert(id);
            for (int childId : childMap.value(id)) {
                stack.append(childId);
            }
        }

        QVector<Book> filtered;
        for (const auto& b : books) {
            if (ids.contains(b.categoryId)) filtered.append(b);
        }
        books = filtered;
    }

    m_bookTable->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        auto& b = books[i];
        m_bookTable->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        m_bookTable->setItem(i, 1, new QTableWidgetItem(b.title));
        m_bookTable->setItem(i, 2, new QTableWidgetItem(b.author));
        m_bookTable->setItem(i, 3, new QTableWidgetItem(b.isbn));
        m_bookTable->setItem(i, 4, new QTableWidgetItem(b.publisher));

        QString catName = b.categoryPath;
        if (catName.isEmpty() && b.categoryId > 0) {
            auto allCats = ApiBridge::instance()->getAllCategories();
            for (const auto& c : allCats) {
                if (c.id == b.categoryId) { catName = c.path; break; }
            }
        }
        m_bookTable->setItem(i, 5, new QTableWidgetItem(catName));
        m_bookTable->setItem(i, 6, new QTableWidgetItem(QString::number(b.totalStock)));
        m_bookTable->setItem(i, 7, new QTableWidgetItem(QString::number(b.availableStock)));
    }
    m_bookTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void BookManagerWidget::onSearch() {
    QString keyword = m_searchInput->text().trimmed();
    if (keyword.isEmpty()) { refreshBookList(); return; }

    auto books = ApiBridge::instance()->searchBooks(keyword);
    m_bookTable->setRowCount(books.size());
    for (int i = 0; i < books.size(); ++i) {
        auto& b = books[i];
        m_bookTable->setItem(i, 0, new QTableWidgetItem(QString::number(b.id)));
        m_bookTable->setItem(i, 1, new QTableWidgetItem(b.title));
        m_bookTable->setItem(i, 2, new QTableWidgetItem(b.author));
        m_bookTable->setItem(i, 3, new QTableWidgetItem(b.isbn));
        m_bookTable->setItem(i, 4, new QTableWidgetItem(b.publisher));

        QString catName = b.categoryPath;
        if (catName.isEmpty() && b.categoryId > 0) {
            auto allCats = ApiBridge::instance()->getAllCategories();
            for (const auto& c : allCats) {
                if (c.id == b.categoryId) { catName = c.path; break; }
            }
        }
        m_bookTable->setItem(i, 5, new QTableWidgetItem(catName));
        m_bookTable->setItem(i, 6, new QTableWidgetItem(QString::number(b.totalStock)));
        m_bookTable->setItem(i, 7, new QTableWidgetItem(QString::number(b.availableStock)));
    }
    m_bookTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void BookManagerWidget::onAddBook() {
    QDialog dlg(this);
    dlg.setWindowTitle("添加图书");
    dlg.setFixedSize(450, 400);

    auto* form = new QFormLayout(&dlg);

    auto* titleEdit = new QLineEdit;
    titleEdit->setPlaceholderText("请输入书名");
    auto* authorEdit = new QLineEdit;
    authorEdit->setPlaceholderText("请输入作者");
    auto* isbnEdit = new QLineEdit;
    isbnEdit->setPlaceholderText("请输入ISBN");
    auto* publisherEdit = new QLineEdit;
    publisherEdit->setPlaceholderText("请输入出版社");
    auto* stockSpin = new QSpinBox;
    stockSpin->setRange(0, 999);
    stockSpin->setValue(1);
    auto* descEdit = new QTextEdit;
    descEdit->setMaximumHeight(80);
    descEdit->setPlaceholderText("可选：图书描述");

    auto* categoryCombo = new QComboBox;
    auto allCats = ApiBridge::instance()->getAllCategories();
    for (const auto& c : allCats) {
        categoryCombo->addItem(c.path.isEmpty() ? c.name : c.path, c.id);
    }

    form->addRow("书名:", titleEdit);
    form->addRow("作者:", authorEdit);
    form->addRow("ISBN:", isbnEdit);
    form->addRow("出版社:", publisherEdit);
    form->addRow("分类:", categoryCombo);
    form->addRow("总库存:", stockSpin);
    form->addRow("描述:", descEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        Book book;
        book.title = titleEdit->text().trimmed();
        book.author = authorEdit->text().trimmed();
        book.isbn = isbnEdit->text().trimmed();
        book.publisher = publisherEdit->text().trimmed();
        book.categoryId = categoryCombo->currentData().toInt();
        book.totalStock = stockSpin->value();
        book.description = descEdit->toPlainText().trimmed();
        for (const auto& c : allCats) {
            if (c.id == book.categoryId) { book.categoryPath = c.path; break; }
        }

        Result r = ApiBridge::instance()->addBook(book);
        if (r.success) {
            QMessageBox::information(this, "成功", "图书添加成功");
            refreshBookList();
            refreshCategoryTree();
        } else {
            QMessageBox::warning(this, "失败", r.message);
        }
    }
}

void BookManagerWidget::onEditBook() {
    int row = m_bookTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择一本书"); return; }

    auto idItem = m_bookTable->item(row, 0);
    int bookId = idItem->text().toInt();
    Book book = ApiBridge::instance()->getBookDetail(bookId);
    if (book.id == 0) { QMessageBox::warning(this, "错误", "图书不存在"); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("编辑图书");
    dlg.setFixedSize(450, 400);

    auto* form = new QFormLayout(&dlg);

    auto* titleEdit = new QLineEdit(book.title);
    auto* authorEdit = new QLineEdit(book.author);
    auto* isbnEdit = new QLineEdit(book.isbn);
    auto* publisherEdit = new QLineEdit(book.publisher);
    auto* stockSpin = new QSpinBox;
    stockSpin->setRange(0, 999);
    stockSpin->setValue(book.totalStock);
    auto* descEdit = new QTextEdit(book.description);
    descEdit->setMaximumHeight(80);

    auto* categoryCombo = new QComboBox;
    auto categories = ApiBridge::instance()->getAllCategories();
    int catIdx = 0;
    for (int i = 0; i < categories.size(); ++i) {
        categoryCombo->addItem(categories[i].path.isEmpty() ? categories[i].name : categories[i].path, categories[i].id);
        if (categories[i].id == book.categoryId) catIdx = i;
    }
    categoryCombo->setCurrentIndex(catIdx);

    form->addRow("书名:", titleEdit);
    form->addRow("作者:", authorEdit);
    form->addRow("ISBN:", isbnEdit);
    form->addRow("出版社:", publisherEdit);
    form->addRow("分类:", categoryCombo);
    form->addRow("总库存:", stockSpin);
    form->addRow("描述:", descEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        book.title = titleEdit->text().trimmed();
        book.author = authorEdit->text().trimmed();
        book.isbn = isbnEdit->text().trimmed();
        book.publisher = publisherEdit->text().trimmed();
        book.categoryId = categoryCombo->currentData().toInt();
        book.totalStock = stockSpin->value();
        book.description = descEdit->toPlainText().trimmed();
        for (const auto& c : categories) {
            if (c.id == book.categoryId) { book.categoryPath = c.path; break; }
        }

        Result r = ApiBridge::instance()->updateBook(book);
        if (r.success) {
            QMessageBox::information(this, "成功", "图书更新成功");
            refreshBookList();
        } else {
            QMessageBox::warning(this, "失败", r.message);
        }
    }
}

void BookManagerWidget::onDeleteBook() {
    int row = m_bookTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选择一本书"); return; }

    auto idItem = m_bookTable->item(row, 0);
    int bookId = idItem->text().toInt();
    auto titleItem = m_bookTable->item(row, 1);
    QString bookTitle = titleItem->text();

    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除《%1》吗？此操作不可恢复。").arg(bookTitle));
    if (reply != QMessageBox::Yes) return;

    Result r = ApiBridge::instance()->deleteBook(bookId);
    if (r.success) {
        QMessageBox::information(this, "成功", "图书已删除");
        refreshBookList();
    } else {
        QMessageBox::warning(this, "失败", r.message);
    }
}

void BookManagerWidget::refreshAll() {
    refreshCategoryTree();
    refreshBookList();
}
