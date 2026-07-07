// test_fine_calculator.cpp — 罚款计算单元测试
#include "backend.h"
#include <cassert>
using namespace std;

int main()
{
    // TODO: 测试 FineCalculator
    // 1. 未逾期(due>return) → assert calcFine == 0
    // 2. 逾期3天 → assert calcFine == 3*0.5 = 1.5
    // 3. 逾期10天 → assert calcFine == 7*0.5 + 3*1.0 = 6.5
    // 4. 逾期35天 → assert calcFine == 7*0.5+23*1.0+5*2.0 = ...
    // 5. 超大逾期 → assert calcFine <= MAX_FINE(100)
    // assert(全部通过)
}
