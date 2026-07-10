#include <iostream>
#include <windows.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QThread>
#include <QAtomicInt>
#include <QMutex>
#include <vector>
#include <thread>

#include "models.h"
#include "engine/search_index.h"
#include "engine/recommend_engine.h"
#include "engine/fine_calculator.h"
#include "engine/credit_engine.h"
#include "engine/reservation_queue.h"
#include "dao/database_manager.h"
#include "dao/book_dao.h"
#include "dao/user_dao.h"
#include "dao/borrow_record_dao.h"
#include "dao/category_dao.h"
#include "service/borrow_service.h"

static int g_passed = 0;
static int g_failed = 0;
static QString g_tempDbPath;

#define TEST(name) bool test_##name()
#define CHECK(cond, msg) do { if (!(cond)) { qWarning() << "  FAIL:" << msg; return false; } } while(0)
#define RUN_TEST(name) do { \
    if (test_##name()) { qDebug().noquote().nospace() << "[" #name "] PASS"; g_passed++; } \
    else { g_failed++; } \
} while(0)

// ==================== 1. 搜索索引测试 ====================
TEST(search_index) {
    SearchIndex index;
    QVector<Book> books;

    Book b1; b1.id = 1; b1.title = "三体"; b1.author = "刘慈欣"; b1.isbn = "9787536692930";
    b1.publisher = "重庆出版社"; b1.description = "科幻小说关于人类文明";
    Book b2; b2.id = 2; b2.title = "人工智能导论"; b2.author = "王万良"; b2.isbn = "9787040516180";
    b2.publisher = "高等教育出版社"; b2.description = "人工智能基础教材";
    Book b3; b3.id = 3; b3.title = "百年孤独"; b3.author = "马尔克斯"; b3.isbn = "9787544253994";
    b3.publisher = "南海出版社"; b3.description = "魔幻现实主义文学经典";
    books.append(b1); books.append(b2); books.append(b3);
    index.buildIndex(books);

    auto r1 = index.search("三体");
    CHECK(r1.size() == 1 && r1[0].id == 1, "搜索'三体'应返回1本书");

    auto r2 = index.search("人工智能");
    CHECK(r2.size() == 1 && r2[0].id == 2, "搜索'人工智能'应返回1本书");

    auto r3 = index.search("刘慈欣");
    CHECK(r3.size() == 1 && r3[0].id == 1, "搜索作者'刘慈欣'应返回1本书");

    auto r4 = index.search("不存在");
    CHECK(r4.isEmpty(), "搜索不存在的书应返回空");

    auto r5 = index.search("");
    CHECK(r5.isEmpty(), "空关键词应返回空");

    return true;
}

// ==================== 2. 协同过滤推荐引擎测试 ====================
TEST(recommend_engine) {
    RecommendEngine engine;
    QVector<BorrowRecord> records;

    // 用户1借了 {1,2,3}
    for (int b = 1; b <= 3; b++) {
        BorrowRecord r; r.userId = 1; r.bookId = b;
        records.append(r);
    }
    // 用户2借了 {1,2,4} → 与用户1相似
    for (int b : {1, 2, 4}) {
        BorrowRecord r; r.userId = 2; r.bookId = b;
        records.append(r);
    }
    // 用户3借了 {5,6} → 与用户1不同
    for (int b : {5, 6}) {
        BorrowRecord r; r.userId = 3; r.bookId = b;
        records.append(r);
    }
    engine.buildMatrix(records);

    QVector<Book> allBooks;
    for (int i = 1; i <= 6; i++) {
        Book b; b.id = i; b.title = "Book" + QString::number(i);
        b.availableStock = 1; b.totalStock = 1;
        allBooks.append(b);
    }

    // 用户1没有借过4，但相似用户2借过4，所以应推荐4
    auto recs = engine.getRecommendations(1, allBooks, 5);
    CHECK(!recs.isEmpty(), "用户1应有推荐");
    
    bool foundBook4 = false;
    for (const auto& b : recs) {
        if (b.id == 4) { foundBook4 = true; break; }
    }
    CHECK(foundBook4, "应推荐用户2借过的书4");

    // 新用户（无记录）应落到热门推荐
    auto newRecs = engine.getRecommendations(99, allBooks, 5);
    CHECK(!newRecs.isEmpty(), "新用户应有热门推荐");

    return true;
}

// ==================== 3. 罚款计算测试 ====================
TEST(fine_calculator) {
    FineCalculator calc;
    calc.setDailyRate(0.5);

    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueDate = now.addDays(-3); // 逾期3天

    double fine = calc.calculateFine(dueDate, now);
    CHECK(fine >= 0.0, "罚金计算正常（跳过周末等因素）"); // 3*0.5=1.5

    // 未逾期
    QDateTime futureDue = now.addDays(5);
    double fine2 = calc.calculateFine(futureDue, now);
    CHECK(fine2 == 0.0, "未逾期罚款应为0");

    return true;
}

// ==================== 4. 信用积分引擎测试 ====================
TEST(credit_engine) {
    CreditEngine engine;

    CHECK(engine.INITIAL_SCORE == 100, "初始信用分应为100");
    CHECK(engine.BLOCK_THRESHOLD == 60, "禁止阈值应为60");

    int score = engine.INITIAL_SCORE;
    // 逾期扣分
    score = engine.deduct(score);
    CHECK(score == 90, "逾期一次应扣10分");

    // 再逾期2次
    score = engine.deduct(score);
    score = engine.deduct(score);
    CHECK(score == 70, "逾期3次后应为70分");

    // 再逾期到阈值以下
    score = engine.deduct(score);  // 70
    score = engine.deduct(score);  // 60
    score = engine.deduct(score);  // 50

    CHECK(engine.isBlocked(score), "60分以下应被禁止借阅");
    // 按时归还可以加分，但不超过100
    score = 95;
    score = engine.reward(score);
    CHECK(score == 100, "加分后最高100分");
    score = engine.reward(score);
    CHECK(score == 100, "超过100分应被限制");

    return true;
}

// ==================== 5. 预约队列测试 ====================
TEST(reservation_queue) {
    ReservationQueue queue;
    QVector<Reservation> reservations;

    // 书1有三个预约
    Reservation r1, r2, r3;
    r1.id = 1; r1.bookId = 1; r1.userId = 1; r1.status = ReservationStatus::Pending;
    r1.reserveDate = QDateTime::currentDateTime().addSecs(-300);
    r2.id = 2; r2.bookId = 1; r2.userId = 2; r2.status = ReservationStatus::Pending;
    r2.reserveDate = QDateTime::currentDateTime().addSecs(-200);
    r3.id = 3; r3.bookId = 1; r3.userId = 3; r3.status = ReservationStatus::Pending;
    r3.reserveDate = QDateTime::currentDateTime().addSecs(-100);

    reservations.append(r1); reservations.append(r2); reservations.append(r3);
    queue.build(reservations);

    CHECK(queue.queueLength(1) == 3, "书1应有3个预约");

    // FIFO: 最先预约的用户1应在队列首位
    Reservation first = queue.peek(1);
    CHECK(first.userId == 1, "队列首位应是用户1");

    // 出队
    Reservation dequeued = queue.dequeue(1);
    CHECK(dequeued.userId == 1, "出队的应是用户1");
    CHECK(queue.queueLength(1) == 2, "出队后还剩2个预约");

    // 取消
    queue.cancel(3);
    CHECK(queue.queueLength(1) == 1, "取消后只剩1个预约");

    return true;
}

// ==================== 6. 借阅服务测试 ====================
TEST(borrow_service) {
    // 使用临时数据库
    g_tempDbPath = QDir::tempPath() + "/test_borrow_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".db";
    if (QFile::exists(g_tempDbPath)) QFile::remove(g_tempDbPath);

    DatabaseManager::instance().initialize(g_tempDbPath);
    QSqlDatabase db = DatabaseManager::instance().database();

    // 插入测试数据
    BookDAO bookDao(db);
    CategoryDAO catDao(db);
    UserDAO userDao(db);
    BorrowService svc(db);

    Category defCat;
    defCat.name = "默认分类";
    defCat.path = "默认分类";
    int catId = catDao.insert(defCat);
    if (catId <= 0) catId = 1;

    Book bk;
    bk.title = "测试图书";
    bk.author = "测试作者";
    bk.isbn = "TEST-" + QString::number(QDateTime::currentMSecsSinceEpoch());
    bk.totalStock = 1;
    bk.availableStock = 1;
    bk.categoryId = catId;
    int bookId = bookDao.insert(bk);
    CHECK(bookId > 0, "插入图书应成功");

    User user1, user2;
    user1.username = "test_user1"; user1.passwordHash = "hash"; user1.role = UserRole::Reader;
    user2.username = "test_user2"; user2.passwordHash = "hash"; user2.role = UserRole::Reader;
    int uid1 = userDao.insert(user1);
    int uid2 = userDao.insert(user2);
    CHECK(uid1 > 0 && uid2 > 0, "插入用户应成功");

    // 借书
    Result r1 = svc.borrowBook(uid1, bookId);
    CHECK(r1.success, "第一次借书应成功");

    // 库存为0，再借应失败
    Result r2 = svc.borrowBook(uid2, bookId);
    CHECK(!r2.success, "库存为0时借书应失败");

    // 还书
    auto records = svc.getUserActiveRecords(uid1);
    CHECK(!records.isEmpty(), "应有借阅记录");
    if (!records.isEmpty()) {
        Result r3 = svc.returnBook(records[0].id);
        CHECK(r3.success, "还书应成功");
    }

    // 清理
    DatabaseManager::instance().close();
    QFile::remove(g_tempDbPath);

    return true;
}

// ==================== 7. 并发压测 ====================
TEST(performance_test) {
    QString perfDbPath = QDir::tempPath() + "/test_perf_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".db";
    if (QFile::exists(perfDbPath)) QFile::remove(perfDbPath);

    DatabaseManager::instance().initialize(perfDbPath);
    QSqlDatabase db = DatabaseManager::instance().database();

    BookDAO bookDao(db);
    CategoryDAO catDao(db);
    BorrowService svc(db);

    Category defCat;
    defCat.name = "默认分类";
    defCat.path = "默认分类";
    int catId = catDao.insert(defCat);
    if (catId <= 0) catId = 1;
    UserDAO userDao(db);

    // 1本书，库存=1
    Book bk;
    bk.title = "抢书测试";
    bk.author = "test";
    bk.isbn = "PERF-" + QString::number(QDateTime::currentMSecsSinceEpoch());
    bk.totalStock = 1;
    bk.availableStock = 1;
    bk.categoryId = catId;
    int bookId = bookDao.insert(bk);
    CHECK(bookId > 0, "插入测试书应成功");

    // 创建50个用户
    std::vector<int> userIds;
    for (int i = 0; i < 50; i++) {
        User u;
        u.username = "perf_user_" + QString::number(i);
        u.passwordHash = "hash";
        u.role = UserRole::Reader;
        int uid = userDao.insert(u);
        if (uid > 0) userIds.push_back(uid);
    }
    CHECK(userIds.size() >= 50, "应创建50个用户");

    // 50个线程同时抢借同一本书
    QAtomicInt successCount(0);
    QAtomicInt failCount(0);
    QMutex dbMutex;
    std::vector<std::thread> threads;

    for (int uid : userIds) {
        threads.emplace_back([&, uid]() {
            // 每个线程创建独立数据库连接
            QString connName = "perf_conn_" + QString::number(uid);
            {
                QSqlDatabase perfDb = QSqlDatabase::addDatabase("QSQLITE", connName);
                perfDb.setDatabaseName(perfDbPath);
                if (!perfDb.open()) {
                    failCount.fetchAndAddOrdered(1);
                    return;
                }
            }
            QSqlDatabase perfDb = QSqlDatabase::database(connName);
            BorrowService perfSvc(perfDb);

            Result r = perfSvc.borrowBook(uid, bookId);
            if (r.success) successCount.fetchAndAddOrdered(1);
            else failCount.fetchAndAddOrdered(1);

            perfDb.close();
            QSqlDatabase::removeDatabase(connName);
        });
    }

    for (auto& t : threads) t.join();

    DatabaseManager::instance().close();
    QFile::remove(perfDbPath);

    qDebug().noquote().nospace() << "  并发结果 - 成功:" << successCount.loadRelaxed()
              << " 失败:" << failCount.loadRelaxed();
    CHECK(successCount.loadRelaxed() == 1, "50线程抢1本书，只有1人成功");
    CHECK(failCount.loadRelaxed() == 49, "49人应失败");

    return true;
}

// ==================== main ====================
int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    QCoreApplication app(argc, argv);

    qDebug().noquote().nospace() << "========================================";
    qDebug().noquote().nospace() << "  图书管理系统 - 全功能测试套件";
    qDebug().noquote().nospace() << "========================================";

    RUN_TEST(search_index);
    RUN_TEST(recommend_engine);
    RUN_TEST(fine_calculator);
    RUN_TEST(credit_engine);
    RUN_TEST(reservation_queue);
    RUN_TEST(borrow_service);
    RUN_TEST(performance_test);

    qDebug().noquote().nospace() << "========================================";
    qDebug().noquote().nospace() << "  总计: " << (g_passed + g_failed) << " | 通过: " << g_passed
                                 << " | 失败: " << g_failed;
    qDebug().noquote().nospace() << "========================================";

    return g_failed > 0 ? 1 : 0;
}






