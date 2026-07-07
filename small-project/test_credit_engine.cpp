// test_credit_engine.cpp — 信用积分单元测试
#include "backend.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
    // 1. 按时还 → 加分
    assert(CreditEngine::updateScore(90, "on_time") == 95);
    // 2. 逾期 → 扣分
    assert(CreditEngine::updateScore(90, "overdue") == 80);
    // 3. 超过100 → 上限
    assert(CreditEngine::updateScore(95, "on_time") == 100);
    // 4. 低于0 → 下限
    assert(CreditEngine::updateScore(5, "overdue") == 0);
    // 5. canBorrow
    assert(CreditEngine::canBorrow(60) == true);
    assert(CreditEngine::canBorrow(59) == false);
    // 6. getLevel
    assert(CreditEngine::getLevel(90) == "excellent");
    assert(CreditEngine::getLevel(75) == "good");
    assert(CreditEngine::getLevel(50) == "poor");
    cout << "All credit engine tests passed!" << endl;
    return 0;
}
