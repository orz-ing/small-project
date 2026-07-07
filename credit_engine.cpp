// credit_engine.cpp — 信用积分引擎
#include "backend.h"
using namespace std;

// 输入: 当前信用分, 操作类型("overdue"/"on_time"/"damage"/"donate")
// 输出: 调整后的信用分（自动限定在 [0,100] 范围）
int CreditEngine::updateScore(int currentScore, const string& action)
{
    // TODO: 根据 action 加减分，clamp 到 [0, MAX_SCORE]
}

// 输入: 信用分值
// 输出: true=≥60分可借阅 | false=不及格禁止借阅
bool CreditEngine::canBorrow(int score)
{
    // TODO: return score >= BORROW_THRESHOLD
}

// 输入: 信用分值
// 输出: "优秀"(≥90) | "良好"(≥75) | "一般"(≥60) | "较差"(60以下)
string CreditEngine::getLevel(int score)
{
    // TODO
}
