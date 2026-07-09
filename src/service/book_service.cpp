#include "book_service.h"
#include <QDebug>

BookService::BookService(QSqlDatabase db) : m_bookDao(db), m_catDao(db) {}

QVector<Book> BookService::getAllBooks() const {
    return m_bookDao.getAll();
}

Book BookService::getBookById(int id) const {
    return m_bookDao.getById(id);
}

Result BookService::addBook(const Book& book) {
    if (book.title.trimmed().isEmpty()) {
        return Result::fail("书名不能为空");
    }
    if (book.totalStock <= 0) {
        return Result::fail("库存必须大于0");
    }

    // 检查ISBN是否重复
    if (!book.isbn.isEmpty()) {
        Book existing = m_bookDao.getByIsbn(book.isbn);
        if (existing.id > 0) {
            return Result::fail("ISBN已存在");
        }
    }

    Book newBook = book;
    newBook.availableStock = book.totalStock;

    // 自动填充分类路径
    if (book.categoryId > 0) {
        Category cat = m_catDao.getById(book.categoryId);
        newBook.categoryPath = cat.path;
    }

    int id = m_bookDao.insert(newBook);
    if (id > 0) {
        return Result::ok("添加成功");
    }
    return Result::fail("添加失败");
}

Result BookService::updateBook(const Book& book) {
    Book existing = m_bookDao.getById(book.id);
    if (existing.id == 0) {
        return Result::fail("图书不存在");
    }

    // 调整available_stock：新旧total_stock的差值
    int stockDiff = book.totalStock - existing.totalStock;
    Book updated = book;
    updated.availableStock = existing.availableStock + stockDiff;
    if (updated.availableStock < 0) updated.availableStock = 0;

    if (book.categoryId > 0) {
        Category cat = m_catDao.getById(book.categoryId);
        updated.categoryPath = cat.path;
    }

    if (m_bookDao.update(updated)) {
        return Result::ok("更新成功");
    }
    return Result::fail("更新失败");
}

Result BookService::deleteBook(int bookId) {
    Book existing = m_bookDao.getById(bookId);
    if (existing.id == 0) {
        return Result::fail("图书不存在");
    }
    if (m_bookDao.deleteBook(bookId)) {
        return Result::ok("删除成功");
    }
    return Result::fail("删除失败");
}

QVector<Book> BookService::getBooksByCategory(int categoryId) const {
    return m_bookDao.getByCategory(categoryId);
}

int BookService::bookCount() const {
    return m_bookDao.count();
}
