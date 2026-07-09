#include "mock_generator.h"
#include "../src/utils.h"
#include "../src/dao/user_dao.h"
#include "../src/dao/book_dao.h"
#include "../src/dao/category_dao.h"
#include "../src/dao/borrow_record_dao.h"
#include "../src/dao/reservation_dao.h"
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QDateTime>
#include <QSqlError>
#include <QHash>

void MockGenerator::generate(int bookCount, int userCount, int borrowCount,
                              int reservationCount, int categoryCount) {
    auto categories = generateCategories(categoryCount);
    auto books = generateBooks(bookCount, categories);
    auto users = generateUsers(userCount);
    auto borrows = generateBorrowRecords(borrowCount, users, books);
    auto reservations = generateReservations(reservationCount, users, books);

    qInfo() << "=== Mock Data Generated ===";
    qInfo() << "Categories:" << categories.size();
    qInfo() << "Books:" << books.size();
    qInfo() << "Users:" << users.size();
    qInfo() << "Borrow Records:" << borrows.size();
    qInfo() << "Reservations:" << reservations.size();
}

QVector<Category> MockGenerator::generateCategories(int count) {
    QVector<Category> result;
    QStringList rootNames = {"文学", "科学", "技术", "艺术", "历史", "哲学", "教育", "经济"};
    QStringList subNames = {"小说", "散文", "诗歌", "物理", "化学", "生物",
                            "计算机", "电子", "机械", "绘画", "音乐", "电影"};

    int id = 1;
    for (int i = 0; i < qMin(count, rootNames.size()); ++i) {
        Category root;
        root.id = id++;
        root.parentId = -1;
        root.name = rootNames[i];
        root.path = root.name;
        root.level = 0;
        result.append(root);

        int subs = QRandomGenerator::global()->bounded(1, 4);
        for (int j = 0; j < subs && id <= count; ++j) {
            Category child;
            child.id = id++;
            child.parentId = root.id;
            child.name = subNames[(i * 3 + j) % subNames.size()];
            child.path = root.name + "/" + child.name;
            child.level = 1;
            result.append(child);
        }
    }
    return result;
}

QVector<Book> MockGenerator::generateBooks(int count, const QVector<Category>& categories) {
    QVector<Book> result;
    QStringList titles = {"数据结构与算法", "计算机网络", "操作系统", "数据库系统概论",
                          "软件工程", "编译原理", "计算机组成原理", "人工智能",
                          "机器学习", "深度学习", "Python编程", "Java核心技术",
                          "C++ Primer", "算法导论", "设计模式", "重构",
                          "人间失格", "百年孤独", "活着", "三体",
                          "红楼梦", "西游记", "三国演义", "水浒传",
                          "平凡的世界", "围城", "月亮与六便士", "小王子"};
    QStringList authors = {"严蔚敏", "谢希仁", "汤子瀛", "王珊",
                           "张海藩", "陈火旺", "唐朔飞", "周志华",
                           "李航", "Ian Goodfellow", "Eric Matthes", "Horstmann",
                           "Stanley Lippman", "Thomas Cormen", "Martin Fowler", "Kent Beck",
                           "太宰治", "马尔克斯", "余华", "刘慈欣",
                           "曹雪芹", "吴承恩", "罗贯中", "施耐庵",
                           "路遥", "钱钟书", "毛姆", "圣埃克苏佩里"};
    QStringList publishers = {"清华大学出版社", "机械工业出版社", "电子工业出版社",
                              "人民邮电出版社", "高等教育出版社", "科学出版社",
                              "中信出版社", "人民文学出版社"};

    for (int i = 0; i < count; ++i) {
        Book b;
        b.id = i + 1;
        b.isbn = QString("978-%1-%2-%3-%4")
            .arg(QRandomGenerator::global()->bounded(7, 9))
            .arg(QRandomGenerator::global()->bounded(100, 999))
            .arg(QRandomGenerator::global()->bounded(1000, 9999))
            .arg(QRandomGenerator::global()->bounded(1000, 9999));
        b.title = titles[i % titles.size()] + (i >= titles.size() ? QString("(%1)").arg(i / titles.size() + 1) : "");
        b.author = authors[i % authors.size()];
        b.publisher = publishers[QRandomGenerator::global()->bounded(publishers.size())];
        b.totalStock = QRandomGenerator::global()->bounded(1, 11);
        b.availableStock = b.totalStock;

        // 按索引映射到不同预设分类，展示所有类别
        QStringList catMap = {
            "技术/计算机", "技术/计算机", "科学/物理",   "技术/计算机",
            "科学/化学",   "技术/电子",   "艺术/绘画",   "技术/计算机",
            "科学/生物",   "技术/机械",   "艺术/音乐",   "技术/计算机",
            "文学/小说",   "文学/散文",   "艺术/电影",   "技术/计算机",
            "文学/诗歌",   "文学/小说",   "文学/小说",   "科学/物理",
            "文学/小说",   "科学/化学",   "文学/散文",   "文学/小说",
            "文学/诗歌",   "科学/生物",   "历史",        "哲学"
        };
        if (!categories.isEmpty()) {
            QString targetPath = catMap[i % catMap.size()];
            int searchIdx = -1;
            for (int ci = 0; ci < categories.size(); ++ci) {
                if (categories[ci].path == targetPath) {
                    searchIdx = ci; break;
                }
            }
            if (searchIdx < 0) {
                // 尝试用最后一段名称匹配
                QString lastName = targetPath.section('/', -1);
                for (int ci = 0; ci < categories.size(); ++ci) {
                    if (categories[ci].name == lastName) {
                        searchIdx = ci; break;
                    }
                }
            }
            if (searchIdx < 0) searchIdx = 0;
            b.categoryId = categories[searchIdx].id;
            b.categoryPath = categories[searchIdx].path;
        }

        b.description = QString("《%1》是一本关于%2的经典著作，由%3编写，%4出版。")
            .arg(b.title).arg("相关领域").arg(b.author).arg(b.publisher);
        b.version = 0;
        b.createTime = QDateTime::currentDateTime().addDays(-QRandomGenerator::global()->bounded(365));

        result.append(b);
    }
    return result;
}

QVector<User> MockGenerator::generateUsers(int count) {
    QVector<User> result;

    User admin;
    admin.username = "admin";
    admin.passwordHash = Utils::sha256("admin123");
    admin.displayName = "系统管理员";
    admin.role = UserRole::Admin;
    admin.creditScore = 100;
    result.append(admin);

    QStringList names = {"张三", "李四", "王五", "赵六", "陈七", "刘八",
                         "孙九", "周十", "吴小明", "郑小红", "Alice", "Bob",
                         "Charlie", "David", "Eva", "Frank", "Grace", "Henry"};

    for (int i = 0; i < qMin(count - 1, (int)names.size()); ++i) {
        User u;
        u.username = "user" + QString::number(i + 1);
        u.passwordHash = Utils::sha256("123456");
        u.displayName = names[i];
        u.role = UserRole::Reader;
        u.creditScore = QRandomGenerator::global()->bounded(50, 101);
        result.append(u);
    }

    return result;
}

QVector<BorrowRecord> MockGenerator::generateBorrowRecords(int count,
    const QVector<User>& users, const QVector<Book>& books) {
    QVector<BorrowRecord> result;

    for (int i = 0; i < count; ++i) {
        BorrowRecord r;
        r.userId = users[QRandomGenerator::global()->bounded(1, users.size())].id;
        r.bookId = books[QRandomGenerator::global()->bounded(books.size())].id;
        r.bookTitle = "";
        r.borrowDate = QDateTime::currentDateTime().addDays(-QRandomGenerator::global()->bounded(60));
        r.dueDate = r.borrowDate.addDays(30);

        bool returned = QRandomGenerator::global()->bounded(100) < 70;
        if (returned) {
            r.returnDate = r.dueDate.addDays(QRandomGenerator::global()->bounded(-5, 15));
            r.status = BorrowStatus::Returned;
            if (r.returnDate > r.dueDate) {
                r.fine = (r.returnDate.daysTo(r.dueDate)) * 0.5 * -1;
                r.fine = qAbs(r.fine);
            }
        } else {
            r.status = BorrowStatus::Borrowing;
        }

        result.append(r);
    }

    return result;
}

QVector<Reservation> MockGenerator::generateReservations(int count,
    const QVector<User>& users, const QVector<Book>& books) {
    QVector<Reservation> result;

    for (int i = 0; i < count; ++i) {
        Reservation r;
        r.userId = users[QRandomGenerator::global()->bounded(1, users.size())].id;
        r.bookId = books[QRandomGenerator::global()->bounded(books.size())].id;
        r.reserveDate = QDateTime::currentDateTime().addDays(-QRandomGenerator::global()->bounded(10));
        r.expireDate = r.reserveDate.addSecs(48 * 3600);
        r.priority = 0;

        int statusRoll = QRandomGenerator::global()->bounded(100);
        if (statusRoll < 50) r.status = ReservationStatus::Pending;
        else if (statusRoll < 65) r.status = ReservationStatus::Notified;
        else if (statusRoll < 80) r.status = ReservationStatus::Cancelled;
        else if (statusRoll < 90) r.status = ReservationStatus::Expired;
        else r.status = ReservationStatus::Fulfilled;

        result.append(r);
    }

    return result;
}

void MockGenerator::generateToDB(QSqlDatabase db, int bookCount, int userCount,
                                  int borrowCount, int reservationCount, int /*categoryCount*/) {
    UserDAO userDao(db);
    CategoryDAO catDao(db);
    BookDAO bookDao(db);
    BorrowRecordDAO borrowDao(db);
    ReservationDAO resDao(db);

    // 1. 从数据库读取已有的预设分类（不再重复创建）
    auto cats = catDao.getAll();

    // 2. 插入图书，直接使用数据库中的真实分类ID
    auto books = generateBooks(bookCount, cats);
    QVector<Book> insertedBooks;
    for (auto& b : books) {
        if (!cats.isEmpty()) {
            // b.categoryId 在 generateBooks 中已设为正确的预设分类ID
            for (const auto& cat : cats) {
                if (cat.id == b.categoryId) {
                    b.categoryPath = cat.path; break;
                }
            }
        } else {
            b.categoryId = 0;
        }
        b.availableStock = b.totalStock;
        int id = bookDao.insert(b);
        if (id > 0) {
            b.id = id;
            insertedBooks.append(b);
        }
    }

    // 3. 插入用户
    auto users = generateUsers(userCount);
    QVector<User> insertedUsers;
    for (auto& u : users) {
        int id = userDao.insert(u);
        if (id > 0) {
            u.id = id;
            insertedUsers.append(u);
        }
    }

    if (insertedBooks.isEmpty() || insertedUsers.isEmpty()) {
        qWarning() << "No data inserted - check DAO";
        return;
    }

    // 4. 插入借阅记录（带书名和ISBN）
    auto records = generateBorrowRecords(borrowCount, insertedUsers, insertedBooks);
    for (auto& r : records) {
        for (const auto& b : insertedBooks) {
            if (b.id == r.bookId) {
                r.bookTitle = b.title;
                r.bookIsbn = b.isbn;
                break;
            }
        }
        int rid = borrowDao.insert(r);
        if (rid > 0 && r.status == BorrowStatus::Borrowing) {
            bookDao.updateStock(r.bookId, -1, 0);
        }
    }

    // 5. 插入预约（带书名）
    auto reservations = generateReservations(reservationCount, insertedUsers, insertedBooks);
    for (auto& r : reservations) {
        for (const auto& b : insertedBooks) {
            if (b.id == r.bookId) {
                r.bookTitle = b.title;
                break;
            }
        }
        resDao.insert(r);
    }
}




