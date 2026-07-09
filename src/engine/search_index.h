#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include "models.h"
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVector>
#include <QPair>
#include <algorithm>

// 倒排索引：将图书分词后建立索引，支持多关键词交集搜索
class SearchIndex {
public:
    SearchIndex() = default;

    // 构建索引
    void buildIndex(const QVector<Book>& books);

    // 搜索：输入关键词，返回按相关度排序的图书列表（最多topN本）
    QVector<Book> search(const QString& keywords, int topN = 20) const;

    // 清空索引
    void clear();

    // 添加/更新/删除单本书
    void addBook(const Book& book);
    void updateBook(const Book& book);
    void removeBook(int bookId);

    int size() const { return m_books.size(); }

private:
    // 分词
    QStringList tokenize(const QString& text) const;

    // 计算单个词的相关度分数
    double calculateScore(const Book& book, const QString& term) const;

    // 倒排索引：词 → 包含该词的图书ID集合
    QHash<QString, QSet<int>> m_invertedIndex;

    // 图书缓存：ID → Book
    QHash<int, Book> m_books;

    // 停用词
    static const QSet<QString> s_stopWords;
};

#endif // SEARCH_INDEX_H
