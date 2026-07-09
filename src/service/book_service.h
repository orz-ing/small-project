#ifndef BOOK_SERVICE_H
#define BOOK_SERVICE_H

#include "models.h"
#include "dao/book_dao.h"
#include "dao/category_dao.h"
#include <QSqlDatabase>

class BookService {
public:
    explicit BookService(QSqlDatabase db);

    QVector<Book> getAllBooks() const;
    Book getBookById(int id) const;
    Result addBook(const Book& book);
    Result updateBook(const Book& book);
    Result deleteBook(int bookId);
    QVector<Book> getBooksByCategory(int categoryId) const;
    int bookCount() const;

private:
    BookDAO m_bookDao;
    CategoryDAO m_catDao;
    QSqlDatabase m_db;
};
#endif
