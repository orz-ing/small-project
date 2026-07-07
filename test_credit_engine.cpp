// test_credit_engine.cpp — 信用积分单元测试
#include "backend.h"
#include <cassert>
using namespace std;

int main()
{
    // TODO: 测试 CreditEngine
    // 1. 按时还 → assert updateScore(90,"on_time") == 95
    // 2. 逾期 → assert updateScore(90,"overdue") == 80
    // 3. 超过100 → assert updateScore(95,"on_time") == 100 (上限)
    // 4. 低于0 → assert updateScore(5,"overdue") == 0 (下限)
    // 5. canBorrow(60) → true; canBorrow(59) → false
    // 6. getLevel(90) → "优秀"; getLevel(75) → "良好"; getLevel(50) → "较差"
    // assert(全部通过)
}
