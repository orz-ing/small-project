#ifndef BOOK_MANAGER_WIDGET_H
#define BOOK_MANAGER_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTreeWidget>
#include <QSplitter>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHash>

class BookManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit BookManagerWidget(QWidget* parent = nullptr);
    void refreshAll();
    bool eventFilter(QObject* watched, QEvent* event) override;
private slots:
    void refreshBookList();
    void refreshCategoryTree();
    void onCategorySelected(QTreeWidgetItem* item, int column);
    void onTreeItemEntered(QTreeWidgetItem* item, int column);
    void onSubcatClicked(int categoryId);
    void onAddBook();
    void onEditBook();
    void onDeleteBook();
    void onSearch();
private:
    void setupUI();
    void showSubcatPopup(QTreeWidgetItem* item, const QPoint& treePos);
    void hideSubcatPopup();

    QTreeWidget* m_categoryTree;
    QTableWidget* m_bookTable;
    QLineEdit* m_searchInput;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    int m_currentCategoryId = -1;

    QFrame* m_subcatPopup = nullptr;
    QVBoxLayout* m_subcatLayout = nullptr;
    int m_hoverRootId = -1;
};
#endif
