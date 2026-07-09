#include "recommend_engine.h"

void RecommendEngine::buildMatrix(const QVector<BorrowRecord>& records) {
    clear();
    for (const auto& rec : records) {
        m_userBooks[rec.userId].insert(rec.bookId);
        m_bookUsers[rec.bookId].insert(rec.userId);
        m_bookBorrowCount[rec.bookId]++;
    }
}

double RecommendEngine::calculateSimilarity(int userA, int userB) const {
    QPair<int,int> key = {qMin(userA, userB), qMax(userA, userB)};
    auto it = m_similarityCache.find(key);
    if (it != m_similarityCache.end()) return it.value();

    auto booksA = m_userBooks.value(userA);
    auto booksB = m_userBooks.value(userB);

    if (booksA.isEmpty() || booksB.isEmpty()) return 0.0;

    // Jaccard系数: |A∩B| / |A∪B|
    int intersection = 0;
    for (int bookId : booksA) {
        if (booksB.contains(bookId)) intersection++;
    }

    int unionSize = booksA.size() + booksB.size() - intersection;
    double similarity = (unionSize > 0) ? (double)intersection / unionSize : 0.0;

    m_similarityCache[key] = similarity;
    return similarity;
}

QVector<Book> RecommendEngine::getRecommendations(int userId, const QVector<Book>& allBooks, int topN) const {
    QHash<int, double> candidateScores; // bookId → score

    // 找到相似用户（借阅过相同书的用户）
    auto userBooks = m_userBooks.value(userId);
    QSet<int> similarUsers;

    for (int bookId : userBooks) {
        auto users = m_bookUsers.value(bookId);
        for (int uid : users) {
            if (uid != userId) similarUsers.insert(uid);
        }
    }

    // 计算候选图书评分
    for (int uid : similarUsers) {
        double sim = calculateSimilarity(userId, uid);
        if (sim <= 0) continue;

        auto otherBooks = m_userBooks.value(uid);
        for (int bookId : otherBooks) {
            if (userBooks.contains(bookId)) continue; // 已借过的不推荐
            candidateScores[bookId] += sim;
        }
    }

    // 排序取TopN
    QVector<QPair<int, double>> sorted;
    for (auto it = candidateScores.begin(); it != candidateScores.end(); ++it) {
        sorted.append({it.key(), it.value()});
    }
    std::sort(sorted.begin(), sorted.end(),
        [](const QPair<int,double>& a, const QPair<int,double>& b) {
            return a.second > b.second;
        });

    // 映射为Book对象
    QHash<int, Book> bookMap;
    for (const auto& b : allBooks) bookMap[b.id] = b;

    QVector<Book> results;
    int count = qMin(topN, sorted.size());
    for (int i = 0; i < count; ++i) {
        auto it = bookMap.find(sorted[i].first);
        if (it != bookMap.end()) {
            results.append(it.value());
        }
    }

    return results;
}

QVector<Book> RecommendEngine::getHotRecommendations(const QVector<Book>& allBooks, int topN) const {
    // 根据借阅次数排序
    QVector<QPair<int, int>> sorted;
    for (auto it = m_bookBorrowCount.begin(); it != m_bookBorrowCount.end(); ++it) {
        sorted.append({it.key(), it.value()});
    }
    std::sort(sorted.begin(), sorted.end(),
        [](const QPair<int,int>& a, const QPair<int,int>& b) {
            return a.second > b.second;
        });

    QHash<int, Book> bookMap;
    for (const auto& b : allBooks) bookMap[b.id] = b;

    QVector<Book> results;
    int count = qMin(topN, sorted.size());
    for (int i = 0; i < count; ++i) {
        auto it = bookMap.find(sorted[i].first);
        if (it != bookMap.end()) {
            results.append(it.value());
        }
    }

    // 如果结果不够，用全部图书补全
    if (results.size() < topN) {
        for (const auto& b : allBooks) {
            if (!m_bookBorrowCount.contains(b.id)) {
                results.append(b);
                if (results.size() >= topN) break;
            }
        }
    }

    return results;
}

void RecommendEngine::clear() {
    m_userBooks.clear();
    m_bookUsers.clear();
    m_bookBorrowCount.clear();
    m_similarityCache.clear();
}
