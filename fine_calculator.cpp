// fine_calculator.cpp — 阶梯费率罚款计算器
#include "backend.h"
using namespace std;

// 输入: dueDate(应还日期), returnDate(实际归还日期), 均为 "YYYY-MM-DD"
// 输出: 罚款金额（元）；0=未逾期；上限为 MAX_FINE
double FineCalculator::calcFine(const string& dueDate, const string& returnDate)
{
    // TODO: daysBetween(due, return) > 0 → 阶梯费率计算
}
