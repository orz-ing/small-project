#include "book_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

BookDAO::BookDAO(QSqlDatabase db) : m_db(db) {}

Book BookDAO::bookFromQuery(QSqlQuery& query) const {
    Book b;
    b.id = query.value("id").toInt();
    b.isbn = query.value("isbn").toString();
    b.title = query.value("title").toString();
    b.author = query.value("author").toString();
    b.publisher = query.value("publisher").toString();
    b.categoryId = query.value("category_id").toInt();
    b.categoryPath = query.value("category_path").toString();
    b.totalStock = query.value("total_stock").toInt();
    b.availableStock = query.value("available_stock").toInt();
    b.version = query.value("version").toInt();
    b.coverPath = query.value("cover_path").toString();
    b.description = query.value("description").toString();
    b.createTime = QDateTime::fromString(query.value("create_time").toString(), Qt::ISODate);
    b.updateTime = QDateTime::fromString(query.value("update_time").toString(), Qt::ISODate);
    return b;
}

QVector<Book> BookDAO::getAll() const {
    QVector<Book> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM books ORDER BY id");
    while (query.next()) {
        result.append(bookFromQuery(query));
    }
    return result;
}

Book BookDAO::getById(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM books WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return bookFromQuery(query);
    }
    return Book{};
}

Book BookDAO::getByIsbn(const QString& isbn) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM books WHERE isbn = ?");
    query.addBindValue(isbn);
    if (query.exec() && query.next()) {
        return bookFromQuery(query);
    }
    return Book{};
}

QVector<Book> BookDAO::getByCategory(int categoryId) const {
    QVector<Book> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM books WHERE category_id = ? ORDER BY id");
    query.addBindValue(categoryId);
    if (query.exec()) {
        while (query.next()) result.append(bookFromQuery(query));
    }
    return result;
}

int BookDAO::insert(const Book& book) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO books (isbn, title, author, publisher, category_id, category_path, "
        "total_stock, available_stock, version, cover_path, description) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?)"
    );
    query.addBindValue(book.isbn);
    query.addBindValue(book.title);
    query.addBindValue(book.author);
    query.addBindValue(book.publisher);
    query.addBindValue(book.categoryId);
    query.addBindValue(book.categoryPath);
    query.addBindValue(book.totalStock);
    query.addBindValue(book.totalStock); // available = total initially
    query.addBindValue(book.coverPath);
    query.addBindValue(book.description);
    if (query.exec()) {
        return query.lastInsertId().toInt();
    }
    qWarning() << "Insert book failed:" << query.lastError().text();
    return -1;
}

bool BookDAO::update(const Book& book) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE books SET isbn=?, title=?, author=?, publisher=?, category_id=?, "
        "category_path=?, total_stock=?, available_stock=?, cover_path=?, description=?, "
        "update_time=datetime('now','localtime') WHERE id=?"
    );
    query.addBindValue(book.isbn);
    query.addBindValue(book.title);
    query.addBindValue(book.author);
    query.addBindValue(book.publisher);
    query.addBindValue(book.categoryId);
    query.addBindValue(book.categoryPath);
    query.addBindValue(book.totalStock);
    query.addBindValue(book.availableStock);
    query.addBindValue(book.coverPath);
    query.addBindValue(book.description);
    query.addBindValue(book.id);
    return query.exec();
}

bool BookDAO::updateStock(int bookId, int delta, int expectedVersion) {
    QSqlQuery query(m_db);
    // 乐观锁：只有 version 匹配时才更新
    query.prepare(
        "UPDATE books SET available_stock = available_stock + ?, "
        "version = version + 1, "
        "update_time = datetime('now','localtime') "
        "WHERE id = ? AND version = ?"
    );
    query.addBindValue(delta);
    query.addBindValue(bookId);
    query.addBindValue(expectedVersion);
    if (query.exec()) {
        return query.numRowsAffected() > 0;
    }
    return false;
}

bool BookDAO::deleteBook(int id) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM books WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

int BookDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM books");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
