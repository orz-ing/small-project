// test_borrow_service.cpp — 借阅事务并发测试
#include "backend.h"
#include <thread>
#include <atomic>
#include <cassert>
using namespace std;

int main()
{
    // TODO: 模拟并发借阅
    // 1. 初始化 DB + BorrowService
    // 2. 准备: 1本书(stock=1)
    // 3. 创建线程A和线程B，同时调用 borrowBook(用户A,书) 和 borrowBook(用户B,书)
    // 4. 统计: 成功次数 == 1 (仅1人成功, 零超卖)
    // 5. 输出测试报告
}
