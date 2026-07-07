// test_fine_calculator.cpp — 罚款计算单元测试
#include "backend.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
    // 1. 未逾期
    assert(FineCalculator::calcFine("2026-06-10", "2026-06-05") == 0.0);
    // 2. 逾期3天
    assert(FineCalculator::calcFine("2026-06-01", "2026-06-04") == 1.5);
    // 3. 逾期10天: 7*0.5 + 3*1.0 = 6.5
    double f = FineCalculator::calcFine("2026-06-01", "2026-06-11");
    assert(f == 6.5);
    // 4. 逾期35天超上限
    double f2 = FineCalculator::calcFine("2026-06-01", "2026-07-06");
    assert(f2 <= 100.0);
    // 5. 超大逾期 → 不超过MAX_FINE
    double f3 = FineCalculator::calcFine("2026-01-01", "2026-12-31");
    assert(f3 <= 100.0);
    cout << "All fine calculator tests passed!" << endl;
    return 0;
}
