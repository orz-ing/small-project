// test_search_index.cpp — 倒排索引单元测试
#include "backend.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
    SearchIndex idx;
    // 构建3本书的索引
    vector<Book> books;
    Book b1; b1.setId(1); b1.setTitle("C++ Programming"); b1.setAuthor("Stroustrup");
    Book b2; b2.setId(2); b2.setTitle("Data Structures"); b2.setAuthor("Weiss");
    Book b3; b3.setId(3); b3.setTitle("C++ Data Structures"); b3.setAuthor("Smith");
    books.push_back(b1); books.push_back(b2); books.push_back(b3);
    idx.buildIndex(books);
    // 搜索 "C++"
    auto r1 = idx.search("C++", 10);
    assert(!r1.empty());
    bool foundCpp = false;
    for (auto& r : r1) if (r.bookId == 1 || r.bookId == 3) foundCpp = true;
    assert(foundCpp);
    // 搜索不存在的
    auto r2 = idx.search("不存在", 10);
    assert(r2.empty());
    // 增量添加/删除
    Book b4; b4.setId(4); b4.setTitle("Python Guide"); b4.setAuthor("Rossum");
    idx.addBook(b4);
    auto r3 = idx.search("Python", 10);
    assert(!r3.empty());
    idx.removeBook(4);
    auto r4 = idx.search("Python", 10);
    assert(r4.empty());
    cout << "All search index tests passed!" << endl;
    return 0;
}
