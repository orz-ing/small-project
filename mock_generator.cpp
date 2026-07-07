// mock_generator.cpp — Mock 数据生成器（独立可执行）
// 用法: ./mock_generator <图书数> <用户数> <借阅数> <预约数> <分类数>
#include "backend.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
using namespace std;

int main(int argc, char* argv[]) {
    int nBooks = 50, nUsers = 20, nBorrows = 100, nReservations = 30, nCategories = 10;
    if (argc >= 2) nBooks = atoi(argv[1]);
    if (argc >= 3) nUsers = atoi(argv[2]);
    if (argc >= 4) nBorrows = atoi(argv[3]);
    if (argc >= 5) nReservations = atoi(argv[4]);
    if (argc >= 6) nCategories = atoi(argv[5]);

    srand((unsigned)time(nullptr));
    DatabaseManager db;
    Status s = db.open("mock_library.db");
    if (!s.ok()) { cerr << "DB open failed: " << s.msg << endl; return 1; }

    UserDao userDao(db);
    BookDao bookDao(db);
    BorrowRecordDao brDao(db);
    ReservationDao resDao(db);
    CategoryDao catDao(db);

    // Generate categories
    vector<int> catIds;
    const char* catNames[] = {"计算机科学","数学","物理","文学","历史","哲学","艺术","经济","生物","化学"};
    for (int i = 0; i < nCategories; i++) {
        Category c; c.setName(catNames[i % 10]); c.setParentId(0); c.setLevel(1);
        Status st; auto created = catDao.insert(c, st);
        if (st.ok()) catIds.push_back(created.getId());
    }

    // Generate books
    vector<int> bookIds;
    const char* bookTitles[] = {"C++程序设计","数据结构","算法导论","计算机网络","操作系统","数据库原理","软件工程","编译原理","人工智能","机器学习"};
    const char* authors[] = {"张三","李四","王五","赵六","钱七","孙八","周九","吴十","郑一","陈二"};
    for (int i = 0; i < nBooks; i++) {
        Book b;
        char isbn[20]; snprintf(isbn, 20, "978-7-%04d-%04d-%d", rand()%10000, rand()%10000, rand()%10);
        b.setIsbn(isbn);
        b.setTitle(string(bookTitles[rand()%10]) + "（第" + to_string(rand()%3+1) + "版）");
        b.setAuthor(authors[rand()%10]);
        b.setPublisher("清华大学出版社");
        b.setCategoryId(catIds.empty() ? 0 : catIds[rand()%catIds.size()]);
        b.setTotalStock(rand()%5+1);
        b.setStock(rand()%5+1);
        b.setLocation(to_string(rand()%5+1) + "楼" + to_string(rand()%10+1) + "架");
        Status st; auto created = bookDao.insert(b, st);
        if (st.ok()) bookIds.push_back(created.getId());
    }

    // Generate users
    vector<int> userIds;
    for (int i = 0; i < nUsers; i++) {
        string uname = "user" + to_string(i+1);
        User u(uname, sha256("123456"), "用户" + to_string(i+1), "reader");
        Status st; auto created = userDao.insert(u, st);
        if (st.ok()) userIds.push_back(created.getId());
    }
    // Admin user
    User admin("admin", sha256("admin123"), "管理员", "admin");
    Status st; userDao.insert(admin, st);

    // Generate borrow records
    for (int i = 0; i < nBorrows; i++) {
        BorrowRecord r;
        r.setUserId(userIds[rand()%userIds.size()]);
        r.setBookId(bookIds[rand()%bookIds.size()]);
        string bd = addDays("2026-01-01", rand()%180);
        r.setBorrowDate(bd);
        r.setDueDate(addDays(bd, 30));
        r.setStatus(rand()%3 == 0 ? "returned" : "borrowing");
        Status st; brDao.insert(r, st);
    }

    // Generate reservations
    for (int i = 0; i < nReservations; i++) {
        Reservation r;
        r.setUserId(userIds[rand()%userIds.size()]);
        r.setBookId(bookIds[rand()%bookIds.size()]);
        r.setReserveDate(today());
        r.setExpireDate(addDays(today(), 2));
        r.setStatus("pending"); r.setPriority(0);
        Status st; resDao.insert(r, st);
    }

    cout << "Mock data generated successfully!\n";
    cout << "  Categories: " << catIds.size() << "\n";
    cout << "  Books: " << bookIds.size() << "\n";
    cout << "  Users: " << userIds.size() << " (+1 admin)\n";
    cout << "  Borrows: " << nBorrows << "\n";
    cout << "  Reservations: " << nReservations << "\n";
    return 0;
}
