#include "credit_engine.h"
#include <QString>

int CreditEngine::deduct(int currentScore, int penalty) const {
    return qMax(0, currentScore - penalty);
}

int CreditEngine::reward(int currentScore, int bonus) const {
    return qMin(MAX_SCORE, currentScore + bonus);
}

bool CreditEngine::isBlocked(int score) const {
    return score < BLOCK_THRESHOLD;
}

QString CreditEngine::getLevelDescription(int score) const {
    if (score >= 90) return QStringLiteral("优秀");
    if (score >= 80) return QStringLiteral("良好");
    if (score >= BLOCK_THRESHOLD) return QStringLiteral("一般");
    if (score >= 30) return QStringLiteral("较差");
    return QStringLiteral("极差");
}
