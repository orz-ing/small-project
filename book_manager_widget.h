#ifndef BOOK_MANAGER_WIDGET_H
#define BOOK_MANAGER_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QList>

class BookManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BookManagerWidget(QWidget *parent = nullptr);

private slots:
    void onSearch();
    void onAddBook();
    void onEditBook();
    void onDeleteBook();

private:
    void setupUI();
    void loadMockData();
    void updateTable();

    QLineEdit   *m_searchEdit;
    QTableWidget *m_table;
    QPushButton *m_addBtn, *m_editBtn, *m_deleteBtn;
    QLabel      *m_statusLabel;

    struct BookItem {
        QString isbn;
        QString title;
        QString author;
        int stock;
    };
    QList<BookItem> m_books;
};

#endif