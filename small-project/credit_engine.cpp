#include "backend.h"

int CreditEngine::updateScore(int currentScore, const string& action) {
    int penalty = 0;
    if (action == "overdue") penalty = PENALTY_OVERDUE;
    else if (action == "on_time") penalty = REWARD_ON_TIME;
    else if (action == "damage") penalty = PENALTY_DAMAGE;
    else if (action == "donate") penalty = REWARD_DONATE;
    else if (action == "severe") penalty = PENALTY_SEVERE;
    else return currentScore;
    return max(0, min(MAX_SCORE, currentScore + penalty));
}

bool CreditEngine::canBorrow(int score) {
    return score >= BORROW_THRESHOLD;
}

string CreditEngine::getLevel(int score) {
    if (score >= 90) return "excellent";
    if (score >= 75) return "good";
    if (score >= 60) return "fair";
    return "poor";
}
