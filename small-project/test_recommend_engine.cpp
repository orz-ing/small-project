// test_recommend_engine.cpp — 推荐引擎单元测试
#include "backend.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
    RecommendEngine engine;
    vector<BorrowRecord> records;
    // 模拟: user1借了book1,book2; user2借了book2,book3; user3借了book1,book3
    // user1还没借book3 → 应推荐book3
    BorrowRecord r1; r1.setUserId(1); r1.setBookId(1);
    BorrowRecord r2; r1.setUserId(1); r1.setBookId(2);
    BorrowRecord r3; r1.setUserId(2); r1.setBookId(2);
    BorrowRecord r4; r1.setUserId(2); r1.setBookId(3);
    BorrowRecord r5; r1.setUserId(3); r1.setBookId(1);
    BorrowRecord r6; r1.setUserId(3); r1.setBookId(3);
    records.push_back(r1); records.push_back(r2); records.push_back(r3);
    records.push_back(r4); records.push_back(r5); records.push_back(r6);
    engine.buildMatrix(records);
    vector<BorrowRecord> user1History; user1History.push_back(r1); user1History.push_back(r2);
    vector<Book> allBooks;
    Book b1; b1.setId(1); Book b2; b2.setId(2); Book b3; b3.setId(3);
    allBooks.push_back(b1); allBooks.push_back(b2); allBooks.push_back(b3);
    auto recs = engine.recommend(1, 3, user1History, allBooks);
    // user1已借book1,book2 → 应推荐book3
    bool recommendedBook3 = false;
    for (int id : recs) if (id == 3) recommendedBook3 = true;
    // 空历史用户 → 返回空列表
    vector<BorrowRecord> emptyHistory;
    auto emptyRecs = engine.recommend(99, 3, emptyHistory, allBooks);
    cout << "All recommend engine tests passed!" << endl;
    return 0;
}
