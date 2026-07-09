#ifndef BOOK_DAO_H
#define BOOK_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class BookDAO {
public:
    explicit BookDAO(QSqlDatabase db);

    QVector<Book> getAll() const;
    Book getById(int id) const;
    Book getByIsbn(const QString& isbn) const;
    QVector<Book> getByCategory(int categoryId) const;
    int insert(const Book& book);
    bool update(const Book& book);
    bool updateStock(int bookId, int delta, int expectedVersion);
    bool deleteBook(int id);
    int count() const;

private:
    Book bookFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};

#endif
