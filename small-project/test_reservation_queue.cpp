// test_reservation_queue.cpp — 预约队列单元测试
#include "backend.h"
#include <cassert>
#include <iostream>
using namespace std;

int main() {
    ReservationQueue q;
    // enqueue
    assert(q.enqueue(1, 1) == 1);
    assert(q.enqueue(1, 2) == 2);
    assert(q.enqueue(1, 1) == -1); // 重复
    // dequeue
    int userId = q.dequeue(1);
    assert(userId == 1); // 先入先出
    // cancel
    q.enqueue(2, 1);
    q.cancel(2, 1);
    assert(q.getPosition(2, 1) == -1);
    // getPosition
    q.enqueue(1, 3);
    assert(q.getPosition(1, 3) != -1);
    // processExpired
    auto expired = q.processExpired(0);
    // size
    assert(q.size(99) == 0);
    cout << "All reservation queue tests passed!" << endl;
    return 0;
}
