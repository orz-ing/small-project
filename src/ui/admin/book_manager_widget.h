#ifndef BOOK_MANAGER_WIDGET_H
#define BOOK_MANAGER_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTreeWidget>
#include <QSplitter>
#include <QLabel>

class BookManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit BookManagerWidget(QWidget* parent = nullptr);
    void refreshAll();
private slots:
    void refreshBookList();
    void refreshCategoryTree();
    void onCategorySelected(QTreeWidgetItem* item, int column);
    void onAddBook();
    void onEditBook();
    void onDeleteBook();
    void onSearch();
private:
    void setupUI();
    QTreeWidget* m_categoryTree;
    QTableWidget* m_bookTable;
    QLineEdit* m_searchInput;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    int m_currentCategoryId = -1;
};
#endif
