#include "backend.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

vector<string> SearchIndex::tokenize(const string& text) const {
    string lower = toLower(text);
    vector<string> tokens; string cur;
    for (char c : lower) {
        if (isalnum(c) || c == '-' || (unsigned char)c >= 0x80) cur += c;
        else { if (cur.size() >= 2) tokens.push_back(cur); cur.clear(); }
    }
    if (cur.size() >= 2) tokens.push_back(cur);
    sort(tokens.begin(), tokens.end());
    tokens.erase(unique(tokens.begin(), tokens.end()), tokens.end());
    return tokens;
}

void SearchIndex::buildIndex(const vector<Book>& books) {
    invertedIndex_.clear();
    borrowCount_.clear();
    for (auto& b : books) addBook(b);
}

vector<SearchResult> SearchIndex::search(const string& query, int topN) const {
    auto tokens = tokenize(query);
    if (tokens.empty()) return {};
    
    unordered_map<int, double> scores;
    for (auto& t : tokens) {
        auto it = invertedIndex_.find(t);
        if (it == invertedIndex_.end()) continue;
        double idf = log((double)(borrowCount_.size() + 1) / (it->second.size() + 1)) + 1.0;
        for (int bookId : it->second) scores[bookId] += idf;
    }
    
    vector<pair<int,double>> sorted(scores.begin(), scores.end());
    sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    
    vector<SearchResult> result;
    for (int i = 0; i < min(topN, (int)sorted.size()); i++)
        result.push_back({sorted[i].first, sorted[i].second});
    return result;
}

void SearchIndex::addBook(const Book& book) {
    auto tokens = tokenize(book.getTitle() + " " + book.getAuthor() + " " + book.getPublisher());
    for (auto& t : tokens) invertedIndex_[t].insert(book.getId());
    if (borrowCount_.find(book.getId()) == borrowCount_.end())
        borrowCount_[book.getId()] = 0;
}

void SearchIndex::removeBook(int bookId) {
    for (auto& [k, v] : invertedIndex_) v.erase(bookId);
    borrowCount_.erase(bookId);
}
