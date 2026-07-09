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
