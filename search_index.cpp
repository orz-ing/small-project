// search_index.cpp — 倒排索引搜索引擎
#include "backend.h"
using namespace std;

// ===== 分词（private）=====
// 输入: 原始文本（书名/作者/简介拼接）
// 输出: 去重去停用词后的关键词列表
vector<string> SearchIndex::tokenize(const string& text) const
{
    // TODO: 转小写 → 按空格/标点分割 → 去重 → 返回
}

// ===== 批量构建索引 =====
// 输入: 全部图书列表
void SearchIndex::buildIndex(const vector<Book>& books)
{
    // TODO: 遍历每本书 → tokenize → 写入 invertedIndex_
}

// ===== 搜索 =====
// 输入: query(查询关键词), topN(返回数量上限)
// 输出: 按相关性得分降序排列的搜索结果列表
vector<SearchResult> SearchIndex::search(const string& query, int topN) const
{
    // TODO: tokenize(query) → 各词结果取交集 → 计算得分 → 排序 → 取 TopN
}

// ===== 增量添加 =====
// 输入: 新图书对象
void SearchIndex::addBook(const Book& book)
{
    // TODO: tokenize → 插入 invertedIndex_
}

// ===== 移除 =====
// 输入: 图书 ID
void SearchIndex::removeBook(int bookId)
{
    // TODO: 从 invertedIndex_ 中逐个关键词移除该 bookId
}
