// test_borrow_service.cpp — 借阅事务并发测试
#include "backend.h"
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    cout << "Borrow service concurrent test - requires DB, skipping unit test." << endl;
    cout << "Run with performance_test for full concurrent test." << endl;
    return 0;
}
