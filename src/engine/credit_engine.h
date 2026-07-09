#ifndef CREDIT_ENGINE_H
#define CREDIT_ENGINE_H

#include <QString>

// 信用积分引擎
class CreditEngine {
public:
    CreditEngine() = default;

    // 初始信用分
    static constexpr int INITIAL_SCORE = 100;
    // 逾期扣分
    static constexpr int OVERDUE_PENALTY = 10;
    // 按时归还加分
    static constexpr int ONTIME_REWARD = 5;
    // 禁止借阅阈值
    static constexpr int BLOCK_THRESHOLD = 60;
    // 最高分
    static constexpr int MAX_SCORE = 100;

    // 计算新的信用分
    int deduct(int currentScore, int penalty = OVERDUE_PENALTY) const;
    int reward(int currentScore, int bonus = ONTIME_REWARD) const;

    // 是否被禁止借阅
    bool isBlocked(int score) const;

    // 获取等级描述
    QString getLevelDescription(int score) const;
};

#endif
