// recommend_engine.cpp — 协同过滤推荐引擎
#include "backend.h"
using namespace std;

// ===== Jaccard 相似度（private）=====
// 输入: 两个图书 ID 集合
// 输出: [0,1] 之间的相似度值
double RecommendEngine::jaccardSimilarity(const set<int>& a, const set<int>& b) const
{
    // TODO: |a∩b| / |a∪b|
}

// ===== 构建共现矩阵 =====
// 输入: 全部借阅记录
void RecommendEngine::buildMatrix(const vector<BorrowRecord>& records)
{
    // TODO: 遍历 records → 同一个用户借过的书两两共现次数 +1
}

// ===== 生成推荐 =====
// 输入: 目标用户ID, 推荐数量, 该用户借阅历史, 全部图书
// 输出: 推荐图书 ID 列表（按推荐分数降序, 排除已借过的）
vector<int> RecommendEngine::recommend(int targetUserId, int topN,
    const vector<BorrowRecord>& userHistory,
    const vector<Book>& allBooks) const
{
    // TODO: 找出相似用户 → 聚合共现矩阵 → 排序 → 取 TopN
}

// ===== 实时更新（新借阅）=====
// 输入: userId, 新借阅的 bookId
void RecommendEngine::onBorrow(int userId, int bookId)
{
    // TODO: 更新 userBooks_ 和 cooccurrence_
}
