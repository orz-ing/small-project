// book_dao.cpp — 图书数据访问对象
#include "backend.h"
using namespace std;

BookDao::BookDao(DatabaseManager& db) : db_(db) {}

Book BookDao::findById(int bookId)
{
    // TODO: SELECT * FROM books WHERE id=?
}

vector<Book> BookDao::findByIds(const vector<int>& ids)
{
    // TODO: SELECT * FROM books WHERE id IN (...)
}

vector<Book> BookDao::findByCategory(int categoryId)
{
    // TODO: SELECT * FROM books WHERE category_id=?
}

vector<Book> BookDao::findAll()
{
    // TODO: SELECT * FROM books
}

// 输入: Book 对象（id 忽略）
// 输出: 新插入的 id; -1=失败
int BookDao::insert(const Book& book)
{
    // TODO: INSERT INTO books(...) VALUES(...)
}

bool BookDao::update(const Book& book)
{
    // TODO: UPDATE books SET ... WHERE id=?
}

bool BookDao::deleteBook(int bookId)
{
    // TODO: DELETE FROM books WHERE id=?
}

// ===== 乐观锁更新库存（核心）=====
// 输入: bookId, delta(+1归还/-1借出), expectedVersion(期望版本号)
// 输出: true=更新成功（版本号匹配）; false=版本冲突需重试
bool BookDao::updateStockWithLock(int bookId, int delta, int expectedVersion)
{
    // TODO:
    // UPDATE books SET stock=stock+(?), version=version+1
    // WHERE id=? AND version=? AND stock+(?) >= 0
    // 检查 affected_rows == 1
}

Book BookDao::rowToBook(void* stmt) const
{
    // TODO: sqlite3 行映射
}
