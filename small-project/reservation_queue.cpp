#include "backend.h"
#include <chrono>

int ReservationQueue::enqueue(int bookId, int userId, int priority) {
    lock_guard<mutex> lock(mtx_);
    auto& q = queues_[bookId];
    // Check if already in queue
    queue<QueueItem> temp;
    bool exists = false;
    while (!q.empty()) {
        auto item = q.front(); q.pop();
        if (item.userId == userId) exists = true;
        temp.push(item);
    }
    q = temp;
    if (exists) return -1;
    
    QueueItem item;
    item.userId = userId;
    item.priority = priority;
    item.timestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
    q.push(item);
    return (int)q.size();
}

int ReservationQueue::dequeue(int bookId) {
    lock_guard<mutex> lock(mtx_);
    auto it = queues_.find(bookId);
    if (it == queues_.end() || it->second.empty()) return -1;
    
    // Find highest priority, oldest item (queue is FIFO within priority)
    // Since we need to reorder, copy to vector, sort, pop best
    vector<QueueItem> items;
    while (!it->second.empty()) {
        items.push_back(it->second.front());
        it->second.pop();
    }
    
    if (items.empty()) return -1;
    
    // Sort: high priority first, then oldest timestamp
    sort(items.begin(), items.end(), [](auto& a, auto& b) {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.timestamp < b.timestamp;
    });
    
    int userId = items[0].userId;
    // Push remaining back
    for (size_t i = 1; i < items.size(); i++)
        it->second.push(items[i]);
    
    return userId;
}

void ReservationQueue::cancel(int bookId, int userId) {
    lock_guard<mutex> lock(mtx_);
    auto it = queues_.find(bookId);
    if (it == queues_.end()) return;
    
    queue<QueueItem> temp;
    while (!it->second.empty()) {
        auto item = it->second.front(); it->second.pop();
        if (item.userId != userId) temp.push(item);
    }
    it->second = temp;
}

int ReservationQueue::getPosition(int bookId, int userId) const {
    lock_guard<mutex> lock(mtx_);
    auto it = queues_.find(bookId);
    if (it == queues_.end()) return -1;
    
    int pos = 1;
    queue<QueueItem> q = it->second;
    while (!q.empty()) {
        if (q.front().userId == userId) return pos;
        q.pop(); pos++;
    }
    return -1;
}

vector<pair<int,int>> ReservationQueue::processExpired(int timeoutHours) {
    lock_guard<mutex> lock(mtx_);
    vector<pair<int,int>> expired;
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    time_t cutoff = now - timeoutHours * 3600;
    
    for (auto& [bookId, q] : queues_) {
        queue<QueueItem> temp;
        while (!q.empty()) {
            auto item = q.front(); q.pop();
            if (item.timestamp < cutoff)
                expired.push_back({bookId, item.userId});
            else
                temp.push(item);
        }
        q = temp;
    }
    return expired;
}

int ReservationQueue::size(int bookId) const {
    lock_guard<mutex> lock(mtx_);
    auto it = queues_.find(bookId);
    return (it == queues_.end()) ? 0 : (int)it->second.size();
}
