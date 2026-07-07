#include "backend.h"
#include <sqlite3.h>

BookDao::BookDao(DatabaseManager& db) : db_(db) {}

Book BookDao::rowToBook(void* stmt) const {
    auto* s = (sqlite3_stmt*)stmt;
    Book b;
    b.setId(sqlite3_column_int(s, 0));
    b.setIsbn(sqlite3_column_text(s,1)?(const char*)sqlite3_column_text(s,1):"");
    b.setTitle((const char*)sqlite3_column_text(s,2));
    b.setAuthor(sqlite3_column_text(s,3)?(const char*)sqlite3_column_text(s,3):"");
    b.setPublisher(sqlite3_column_text(s,4)?(const char*)sqlite3_column_text(s,4):"");
    b.setPublishDate(sqlite3_column_text(s,5)?(const char*)sqlite3_column_text(s,5):"");
    b.setCategoryId(sqlite3_column_int(s,6));
    b.setStock(sqlite3_column_int(s,7));
    b.setTotalStock(sqlite3_column_int(s,8));
    b.setVersion(sqlite3_column_int(s,9));
    b.setLocation(sqlite3_column_text(s,10)?(const char*)sqlite3_column_text(s,10):"");
    b.setDescription(sqlite3_column_text(s,11)?(const char*)sqlite3_column_text(s,11):"");
    b.setCoverUrl(sqlite3_column_text(s,12)?(const char*)sqlite3_column_text(s,12):"");
    return b;
}

Book BookDao::findById(int bookId) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "SELECT * FROM books WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return Book();
    sqlite3_bind_int(stmt, 1, bookId);
    Book b;
    if (sqlite3_step(stmt) == SQLITE_ROW) b = rowToBook(stmt);
    sqlite3_finalize(stmt);
    return b;
}

vector<Book> BookDao::findByIds(const vector<int>& ids) {
    vector<Book> books;
    if (ids.empty()) return books;
    auto* db = (sqlite3*)db_.getHandle();
    string sql = "SELECT * FROM books WHERE id IN (";
    for (size_t i = 0; i < ids.size(); i++) {
        if (i > 0) sql += ",";
        sql += "?";
    }
    sql += ")";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return books;
    for (size_t i = 0; i < ids.size(); i++)
        sqlite3_bind_int(stmt, (int)(i+1), ids[i]);
    while (sqlite3_step(stmt) == SQLITE_ROW) books.push_back(rowToBook(stmt));
    sqlite3_finalize(stmt);
    return books;
}

vector<Book> BookDao::findByCategory(int categoryId) {
    auto* db = (sqlite3*)db_.getHandle();
    vector<Book> books;
    const char* sql = "SELECT * FROM books WHERE category_id=? ORDER BY id";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return books;
    sqlite3_bind_int(stmt, 1, categoryId);
    while (sqlite3_step(stmt) == SQLITE_ROW) books.push_back(rowToBook(stmt));
    sqlite3_finalize(stmt);
    return books;
}

vector<Book> BookDao::findAll() {
    auto* db = (sqlite3*)db_.getHandle();
    vector<Book> books;
    const char* sql = "SELECT * FROM books ORDER BY id";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return books;
    while (sqlite3_step(stmt) == SQLITE_ROW) books.push_back(rowToBook(stmt));
    sqlite3_finalize(stmt);
    return books;
}

Book BookDao::insert(const Book& book, Status& status) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "INSERT INTO books (isbn,title,author,publisher,publish_date,category_id,stock,total_stock,version,location,description,cover_url) VALUES (?,?,?,?,?,?,?,?,1,?,?,?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        status = {StatusCode::ERR_DB_EXEC, "prepare insert book failed"};
        return Book();
    }
    sqlite3_bind_text(stmt,1,book.getIsbn().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,2,book.getTitle().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,3,book.getAuthor().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,4,book.getPublisher().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,5,book.getPublishDate().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_int(stmt,6,book.getCategoryId());
    sqlite3_bind_int(stmt,7,book.getStock());
    sqlite3_bind_int(stmt,8,book.getTotalStock());
    sqlite3_bind_text(stmt,9,book.getLocation().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,10,book.getDescription().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,11,book.getCoverUrl().c_str(),-1,SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        status = {StatusCode::ERR_UNKNOWN, "insert book failed"};
        sqlite3_finalize(stmt);
        return Book();
    }
    int id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    Book created = book;
    created.setId(id);
    created.setVersion(1);
    status = StatusOK();
    return created;
}

Status BookDao::update(const Book& book) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "UPDATE books SET isbn=?,title=?,author=?,publisher=?,publish_date=?,category_id=?,stock=?,total_stock=?,location=?,description=?,cover_url=? WHERE id=?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {StatusCode::ERR_DB_EXEC, "prepare update book failed"};
    sqlite3_bind_text(stmt,1,book.getIsbn().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,2,book.getTitle().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,3,book.getAuthor().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,4,book.getPublisher().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,5,book.getPublishDate().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_int(stmt,6,book.getCategoryId());
    sqlite3_bind_int(stmt,7,book.getStock());
    sqlite3_bind_int(stmt,8,book.getTotalStock());
    sqlite3_bind_text(stmt,9,book.getLocation().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,10,book.getDescription().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,11,book.getCoverUrl().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_int(stmt,12,book.getId());
    int rc = sqlite3_step(stmt); sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? StatusOK() : Status{StatusCode::ERR_UNKNOWN, "update book failed"};
}

Status BookDao::deleteBook(int bookId) {
    return db_.execute("DELETE FROM books WHERE id=" + to_string(bookId));
}

Status BookDao::updateStockWithLock(int bookId, int delta, int expectedVersion) {
    auto* db = (sqlite3*)db_.getHandle();
    const char* sql = "UPDATE books SET stock=stock+?,version=version+1 WHERE id=? AND version=? AND stock+?>=0";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return {StatusCode::ERR_DB_EXEC, "prepare stock lock failed"};
    sqlite3_bind_int(stmt, 1, delta);
    sqlite3_bind_int(stmt, 2, bookId);
    sqlite3_bind_int(stmt, 3, expectedVersion);
    sqlite3_bind_int(stmt, 4, delta);
    int rc = sqlite3_step(stmt);
    int affected = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return {StatusCode::ERR_DB_EXEC, "stock update failed"};
    if (affected == 0) return {StatusCode::ERR_VERSION_CONFLICT, "stock version conflict"};
    return StatusOK();
}
