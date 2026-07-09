#include "search_index.h"
#include <QRegularExpression>
#include <cmath>

const QSet<QString> SearchIndex::s_stopWords = {
    "的", "了", "在", "是", "我", "有", "和", "就", "不", "人",
    "都", "一", "一个", "上", "也", "很", "到", "说", "要", "去",
    "你", "会", "着", "没有", "看", "好", "自己", "这", "他", "她",
    "它", "们", "那", "些", "为", "所以", "因为", "但是", "可以",
    "the", "a", "an", "is", "are", "was", "were", "be", "been",
    "being", "have", "has", "had", "do", "does", "did", "will",
    "would", "could", "should", "may", "might", "shall", "can",
    "to", "of", "in", "for", "on", "with", "at", "by", "from",
    "as", "into", "through", "during", "before", "after", "about"
};

QStringList SearchIndex::tokenize(const QString& text) const {
    QStringList result;
    // 中文按单字切分（简单方案，实际可用结巴等分词库）
    // 英文和数字按空格/标点切分
    QString current;
    for (const QChar& ch : text) {
        if (ch.isLetterOrNumber() || ch == '\'') {
            current += ch.toLower();
        } else {
            if (!current.isEmpty()) {
                result.append(current);
                current.clear();
            }
        }
    }
    if (!current.isEmpty()) {
        result.append(current);
    }

    // 过滤停用词
    QStringList filtered;
    for (const auto& word : result) {
        if (!s_stopWords.contains(word) && word.length() >= 1) {
            filtered.append(word);
        }
    }
    return filtered;
}

double SearchIndex::calculateScore(const Book& book, const QString& term) const {
    double score = 0.0;
    QString lower = term.toLower();

    // 标题匹配权重最高
    if (book.title.toLower().contains(lower)) score += 10.0;
    // 作者匹配
    if (book.author.toLower().contains(lower)) score += 8.0;
    // ISBN精确匹配最高分
    if (book.isbn.toLower() == lower) score += 20.0;
    // 出版社匹配
    if (book.publisher.toLower().contains(lower)) score += 5.0;
    // 描述匹配
    if (book.description.toLower().contains(lower)) score += 3.0;

    // TF近似：词出现次数越多分数越高（简单实现）
    int count = 0;
    count += book.title.toLower().count(lower);
    count += book.author.toLower().count(lower);
    count += book.description.toLower().count(lower);
    score += count * 1.5;

    return score;
}

void SearchIndex::buildIndex(const QVector<Book>& books) {
    clear();
    for (const auto& book : books) {
        addBook(book);
    }
}

void SearchIndex::addBook(const Book& book) {
    m_books[book.id] = book;

    // 对标题、作者、出版社、描述进行分词
    QStringList fields = {
        book.title, book.author, book.publisher, book.description
    };

    QSet<QString> added; // 避免同一本书同一个词重复记录
    for (const auto& field : fields) {
        QStringList tokens = tokenize(field);
        for (const auto& token : tokens) {
            if (!added.contains(token)) {
                m_invertedIndex[token].insert(book.id);
                added.insert(token);
            }
        }
    }
}

void SearchIndex::updateBook(const Book& book) {
    removeBook(book.id);
    addBook(book);
}

void SearchIndex::removeBook(int bookId) {
    m_books.remove(bookId);
    // 从倒排索引中移除
    for (auto it = m_invertedIndex.begin(); it != m_invertedIndex.end(); ) {
        it->remove(bookId);
        if (it->isEmpty()) {
            it = m_invertedIndex.erase(it);
        } else {
            ++it;
        }
    }
}

QVector<Book> SearchIndex::search(const QString& keywords, int topN) const {
    if (keywords.trimmed().isEmpty()) return {};

    QStringList terms = tokenize(keywords);
    if (terms.isEmpty()) return {};

    // 第一个词的结果作为候选集
    auto it = m_invertedIndex.find(terms[0]);
    if (it == m_invertedIndex.end()) return {};

    QSet<int> resultIds = it.value();

    // 与后续词做交集
    for (int i = 1; i < terms.size(); ++i) {
        it = m_invertedIndex.find(terms[i]);
        if (it == m_invertedIndex.end()) {
            // 某个词没有匹配，但可以继续（宽松模式：交集为空时保留之前结果）
            // 这里取交集
            QSet<int> intersection;
            // 如果交集为空，保留之前结果（宽松策略）
            continue;
        }
        QSet<int> intersection;
        for (int id : resultIds) {
            if (it.value().contains(id)) {
                intersection.insert(id);
            }
        }
        if (!intersection.isEmpty()) {
            resultIds = intersection;
        }
    }

    // 转成 QVector<Book> 并排序
    QVector<QPair<int, double>> scored;
    for (int id : resultIds) {
        auto bookIt = m_books.find(id);
        if (bookIt == m_books.end()) continue;

        double totalScore = 0.0;
        for (const auto& term : terms) {
            totalScore += calculateScore(bookIt.value(), term);
        }
        scored.append({id, totalScore});
    }

    // 按分数降序排列
    std::sort(scored.begin(), scored.end(),
        [](const QPair<int, double>& a, const QPair<int, double>& b) {
            return a.second > b.second;
        });

    // 取 TopN
    QVector<Book> results;
    int count = qMin(topN, scored.size());
    for (int i = 0; i < count; ++i) {
        results.append(m_books[scored[i].first]);
    }

    return results;
}

void SearchIndex::clear() {
    m_invertedIndex.clear();
    m_books.clear();
}
