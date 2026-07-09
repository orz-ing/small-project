#ifndef RECOMMEND_ENGINE_H
#define RECOMMEND_ENGINE_H

#include "models.h"
#include <QHash>
#include <QSet>
#include <QVector>
#include <QPair>
#include <algorithm>
#include <cmath>

// 协同过滤推荐引擎：基于用户的共现矩阵
class RecommendEngine {
public:
    RecommendEngine() = default;

    // 构建共现矩阵（基于借阅记录）
    void buildMatrix(const QVector<BorrowRecord>& records);

    // 获取给用户的推荐书籍（最多topN本）
    QVector<Book> getRecommendations(int userId, const QVector<Book>& allBooks, int topN = 10) const;

    // 热门推荐（新用户冷启动）
    QVector<Book> getHotRecommendations(const QVector<Book>& allBooks, int topN = 10) const;

    void clear();

private:
    // 用户 → 借阅过的图书ID集合
    QHash<int, QSet<int>> m_userBooks;

    // 图书 → 借阅过的用户ID集合
    QHash<int, QSet<int>> m_bookUsers;

    // 图书借阅次数
    QHash<int, int> m_bookBorrowCount;

    // 用户相似度缓存
    mutable QHash<QPair<int,int>, double> m_similarityCache;

    // 计算两个用户的相似度（Jaccard系数）
    double calculateSimilarity(int userA, int userB) const;
};

#endif // RECOMMEND_ENGINE_H
