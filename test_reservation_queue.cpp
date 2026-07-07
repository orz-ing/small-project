// test_reservation_queue.cpp — 预约队列单元测试
#include "backend.h"
#include <cassert>
using namespace std;

int main()
{
    // TODO: 测试 ReservationQueue
    // 1. enqueue(book1, user1) → assert position == 1
    // 2. enqueue(book1, user2) → assert position == 2
    // 3. enqueue(book1, user1) → assert position == -1 (重复)
    // 4. cancel(book1, user1) → assert getPosition == -1
    // 5. dequeue(book1) → assert == user2
    // 6. processExpired(0) → 验证超时预约被清理
    // assert(全部通过)
}
