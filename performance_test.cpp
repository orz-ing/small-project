// performance_test.cpp — 并发压测工具（独立可执行）
// 功能: 100线程并发抢最后一本书 → 验证乐观锁零超卖
#include "backend.h"
#include <thread>
using namespace std;

int main()
{
    // TODO:
    // 1. 初始化 DB + 各 Service
    // 2. 准备: 插入1本书(stock=1), 100个用户
    // 3. 创建100个线程，每个线程调用 borrowSvc->borrowBook(userId, bookId)
    // 4. 统计成功/失败次数 → 成功次数应恰好为1
    // 5. 输出测试报告: "并发测试: 100线程抢1本书 → 成功X次 | 结果: PASS/FAIL"
}
