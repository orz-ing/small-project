#include "backend.h"
#include <algorithm>
#include <cmath>

double RecommendEngine::jaccardSimilarity(const set<int>& a, const set<int>& b) const {
    if (a.empty() && b.empty()) return 0.0;
    set<int> inter, uni;
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(inter, inter.begin()));
    set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(uni, uni.begin()));
    return (double)inter.size() / (double)uni.size();
}

void RecommendEngine::buildMatrix(const vector<BorrowRecord>& records) {
    userBooks_.clear();
    cooccurrence_.clear();
    for (auto& r : records) {
        int uid = r.getUserId(), bid = r.getBookId();
        userBooks_[uid].insert(bid);
    }
    // Build co-occurrence: for each pair of books borrowed by same user
    for (auto& [uid, books] : userBooks_) {
        for (auto it = books.begin(); it != books.end(); ++it) {
            for (auto jt = next(it); jt != books.end(); ++jt) {
                cooccurrence_[{min(*it, *jt), max(*it, *jt)}]++;
            }
        }
    }
}

vector<int> RecommendEngine::recommend(int targetUserId, int topN,
                                       const vector<BorrowRecord>& userHistory,
                                       const vector<Book>& allBooks) const {
    set<int> borrowed;
    for (auto& r : userHistory) borrowed.insert(r.getBookId());
    
    // Score each un-borrowed book by similarity to borrowed books
    map<int, double> scores;
    for (auto& [uid, books] : userBooks_) {
        if (uid == targetUserId) continue;
        double sim = jaccardSimilarity(borrowed, books);
        if (sim <= 0.0) continue;
        for (int bid : books) {
            if (borrowed.find(bid) == borrowed.end())
                scores[bid] += sim;
        }
    }
    
    vector<pair<int,double>> sorted(scores.begin(), scores.end());
    sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    
    vector<int> result;
    for (int i = 0; i < min(topN, (int)sorted.size()); i++)
        result.push_back(sorted[i].first);
    return result;
}

void RecommendEngine::onBorrow(int userId, int bookId) {
    userBooks_[userId].insert(bookId);
    for (auto& [uid, books] : userBooks_) {
        if (uid == userId) continue;
        if (books.find(bookId) != books.end()) continue;
        for (int otherBook : books) {
            auto key = make_pair(min(bookId, otherBook), max(bookId, otherBook));
            cooccurrence_[key]++;
        }
    }
}
