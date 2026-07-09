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
#include <QPainter>
#include <QPixmap>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
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
    // 确保封面目录存在（保存到 exe 同级的 resources/covers/）
    QString coverBaseDir = QCoreApplication::applicationDirPath() + "/resources/covers";
    QDir coverDir(coverBaseDir);
    if (!coverDir.exists()) {
        QDir().mkpath(coverBaseDir);
    }
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
        b.title = titles[i % titles.size()] + (i >= titles.size() ? QString(" (%1)").arg(i / titles.size() + 1) : "");
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

        // 精确简介映射表（书名|作者 → 简介）
        static const QHash<QString, QString> descMap = {
            {"数据结构与算法|严蔚敏",
             "国内计算机专业经典教材，系统讲解线性表、树、图等数据结构以及查找、排序等算法，"
             "注重理论与实践结合，是数据结构入门的必读之作。"},
            {"计算机网络|谢希仁",
             "以TCP/IP体系为核心，从物理层到应用层逐层阐述网络原理与协议，"
             "内容清晰易懂，是国内多数高校计算机网络课程的指定教材。"},
            {"操作系统|汤子瀛",
             "深入介绍进程管理、内存管理、文件系统和设备管理，结合主流操作系统实例，"
             "帮助读者理解操作系统内核的设计思路与实现方法。"},
            {"数据库系统概论|王珊",
             "系统讲述关系数据库理论、SQL语言、数据库设计及安全性完整性控制，"
             "王珊教授主编的该教材长期被用作数据库入门教学的标准用书。"},
            {"软件工程|张海藩",
             "覆盖软件生命周期各阶段的方法与工具，包括需求分析、设计、测试和维护，"
             "强调工程化思想，是软件工程领域广受推崇的经典教材。"},
            {"编译原理|陈火旺",
             "从词法分析、语法分析到中间代码生成与优化，全面剖析编译器构造技术，"
             "陈火旺教授主编的版本以严谨著称，是编译课程权威参考书。"},
            {"计算机组成原理|唐朔飞",
             "系统介绍计算机各部件的工作原理，包括运算器、控制器、存储器及输入输出系统，"
             "内容深入浅出，是理解计算机硬件结构的必读书目。"},
            {"人工智能|周志华",
             "周志华教授为计算机专业学生编写的人工智能教材，涵盖知识表示、搜索推理、"
             "机器学习等核心内容，体现了人工智能领域的经典理论与新进展。"},
            {"机器学习|李航",
             "聚焦统计学习方法的经典著作，详尽推导了支持向量机、提升方法、隐马尔可夫模型等算法，"
             "数学严谨、示例丰富，是机器学习研究者常备案头之作。"},
            {"深度学习|Ian Goodfellow",
             "深度学习领域奠基性教材，由Goodfellow等人合著，系统讲解数学基础、深度网络模型及优化方法，"
             "被誉为人工智能时代的「花书」。"},
            {"Python编程|Eric Matthes",
             "面向零基础读者的Python入门教程，从基本语法到实战项目循序渐进，"
             "风格通俗易懂，是学习Python编程的理想实践指南。"},
            {"Java核心技术|Horstmann",
             "Java领域的百科全书式著作，深入剖析Java SE的核心特性、类库及高级编程技术，"
             "结构清晰、示例经典，备受Java开发者推崇。"},
            {"C++ Primer|Stanley Lippman",
             "C++教程中的「圣经」，从基础语法到面向对象、泛型编程逐步展开，"
             "全面覆盖C++标准库，适合初学者精读及有经验者查阅。"},
            {"算法导论|Thomas Cormen",
             "算法领域的权威之作，深入分析排序、图算法、动态规划等各类算法，"
             "兼顾正确性证明与复杂度分析，是全球顶尖高校的通用教材。"},
            {"设计模式|Martin Fowler",
             "Martin Fowler通过丰富的实例剖析软件设计中常见的设计模式与架构原则，"
             "帮助开发者掌握可复用的解决方案，提升系统的灵活性与可维护性。"},
            {"重构|Kent Beck",
             "代码改善的经典指南，Kent Beck定义了「代码坏味」并系统讲解各项重构手法，"
             "引导程序员在不改变程序外部行为的前提下，持续优化代码结构。"},
            {"人间失格|太宰治",
             "太宰治的半自传体小说，以主人公叶藏的手记形式，"
             "描摹了边缘人无法融入社会的挣扎与绝望，是日本文学中极具感染力的悲剧名篇。"},
            {"百年孤独|马尔克斯",
             "魔幻现实主义代表作，以布恩迪亚家族七代人的传奇故事，映射拉丁美洲的百年沧桑。"
             "宏伟的叙事与瑰丽的想象，使其成为不朽的文学经典。"},
            {"活着|余华",
             "讲述农民福贵在时代洪流中接连失去亲人却依然坚韧活着的故事。"
             "语言克制而悲怆，以个体的苦难折射生命的韧性与存在的意义。"},
            {"三体|刘慈欣",
             "中国科幻里程碑，从地球文明与三体世界的碰撞展开，构筑了黑暗森林法则等宇宙社会学图景。"
             "恢弘的想象与硬核的科学推演震撼了世界。"},
            {"红楼梦|曹雪芹",
             "中国古典小说巅峰之作，以贾宝玉、林黛玉的悲剧爱情为线索，描绘封建大家族的兴衰，"
             "包罗诗词、医药、饮食等万象，意蕴深邃无尽。"},
            {"西游记|吴承恩",
             "浪漫主义神魔小说，讲述唐僧师徒四人西天取经的历险故事。"
             "孙悟空等形象深入人心，融合奇幻、诙谐与抗争精神，影响深远。"},
            {"三国演义|罗贯中",
             "历史演义开山之作，以东汉末年至西晋初年的群雄争霸为背景，"
             "刻画了诸葛亮、曹操等群像，权谋与忠义交织，充满史诗气魄。"},
            {"水浒传|施耐庵",
             "英雄传奇小说的典范，描写一百零八位好汉被逼上梁山、揭竿而起的故事。"
             "「官逼民反」的主题与鲜活的人物群像，构成一幅宋代市井生活长卷。"},
            {"平凡的世界|路遥",
             "以70至80年代陕北黄土高原为背景，通过孙少安、孙少平两兄弟的奋斗与爱情，"
             "全景展现普通人在大时代中艰难跋涉、追求尊严的生命历程。"},
            {"围城|钱钟书",
             "钱钟书以犀利幽默的笔触，借方鸿渐的人生漂泊，揭示婚姻、事业与人生的「围城」困境。"
             "妙喻迭出，是中国现代文学中独树一帜的讽刺奇书。"},
            {"月亮与六便士|毛姆",
             "以画家高更为原型，讲述证券经纪人思特里克兰德抛弃家庭去追寻艺术梦想的故事，"
             "深刻探讨了现实与理想、世俗与天才之间的激烈冲突。"},
            {"小王子|圣埃克苏佩里",
             "以飞行员与小王子在撒哈拉的相遇为引，用童话笔触揭示爱与责任、孤独与羁绊的哲理。"
             "文字纯净忧伤，感动了全世界无数成人与儿童。"},
        };

        QString descKey = b.title + "|" + b.author;
        if (descMap.contains(descKey)) {
            b.description = descMap[descKey];
        } else {
            // 兜底：超出28本时用模板生成
            QString fieldName = b.categoryPath.section('/', -1);
            if (fieldName.isEmpty()) fieldName = "经典";
            b.description = QString("《%1》是%2领域不可多得的一本好书，值得细细品读。")
                .arg(b.title, fieldName);
        }
        b.version = 0;
        b.createTime = QDateTime::currentDateTime().addDays(-QRandomGenerator::global()->bounded(365));

        // 生成精美封面图片
        {
            const QColor palette[][2] = {
                {QColor("#1a3d6b"), QColor("#2d6a9f")}, {QColor("#7b1d3d"), QColor("#b82d5a")},
                {QColor("#1a6b3d"), QColor("#2d9f5e")}, {QColor("#6b501a"), QColor("#9f7a2d")},
                {QColor("#3d1a6b"), QColor("#6a2d9f")}, {QColor("#1a506b"), QColor("#2d809f")},
                {QColor("#6b301a"), QColor("#9f502d")}, {QColor("#4a1a6b"), QColor("#7a2d9f")},
                {QColor("#1a6b5a"), QColor("#2d9f82")}, {QColor("#556b1a"), QColor("#8a9f2d")},
            };
            int ci = i % 10;
            QColor top = palette[ci][0], bot = palette[ci][1];

            QPixmap cover(260, 380);
            cover.fill(Qt::transparent);
            QPainter p(&cover);
            p.setRenderHint(QPainter::Antialiasing);

            // 背景渐变
            QLinearGradient grad(0, 0, 0, 380);
            grad.setColorAt(0, top);
            grad.setColorAt(1, bot);
            p.setPen(Qt::NoPen);
            p.setBrush(grad);
            p.drawRoundedRect(0, 0, 260, 380, 10, 10);

            // 书脊装饰
            p.setBrush(QColor(255, 255, 255, 25));
            p.drawRoundedRect(12, 20, 6, 340, 3, 3);

            // 顶部装饰条
            p.setBrush(QColor(255, 255, 255, 15));
            p.drawRect(30, 55, 200, 1);

            // 书名文字
            QString titleText = b.title.trimmed();
            QFont titleFont("Microsoft YaHei", 22, QFont::Bold);
            p.setFont(titleFont);
            p.setPen(QColor(255, 255, 255));

            if (titleText.length() <= 4) {
                p.drawText(QRectF(30, 65, 200, 200), Qt::AlignHCenter | Qt::AlignTop, titleText);
            } else if (titleText.length() <= 8) {
                int mid = titleText.length() / 2;
                // 按整字拆分
                QString l1 = titleText.left(mid), l2 = titleText.mid(mid);
                p.drawText(QRectF(30, 75, 200, 45), Qt::AlignHCenter | Qt::AlignBottom, l1);
                p.drawText(QRectF(30, 118, 200, 45), Qt::AlignHCenter | Qt::AlignTop, l2);
            } else {
                titleFont.setPixelSize(20);
                p.setFont(titleFont);
                QString l1 = titleText.left(4), l2 = titleText.mid(4, 4);
                p.drawText(QRectF(30, 75, 200, 45), Qt::AlignHCenter | Qt::AlignBottom, l1);
                p.drawText(QRectF(30, 118, 200, 45), Qt::AlignHCenter | Qt::AlignTop, l2);
            }

            // 分隔线
            p.setBrush(QColor(255, 255, 255, 60));
            p.drawRoundedRect(100, 210, 60, 1, 1, 1);

            // 作者名
            QFont authorFont("Microsoft YaHei", 13);
            p.setFont(authorFont);
            p.setPen(QColor(255, 255, 255, 210));
            QString authorText = b.author;
            if (authorText.length() > 8) authorText = authorText.left(8) + "…";
            p.drawText(QRectF(30, 220, 200, 35), Qt::AlignHCenter, authorText);

            // 出版社（底部小字）
            QFont pubFont("Microsoft YaHei", 9);
            p.setFont(pubFont);
            p.setPen(QColor(255, 255, 255, 160));
            QString pubText = b.publisher;
            p.drawText(QRectF(30, 320, 200, 40), Qt::AlignHCenter | Qt::AlignBottom, pubText);

            p.end();

            QString coverPath = coverDir.absoluteFilePath(QString("book_%1.png").arg(i));
            cover.save(coverPath, "PNG");
            b.coverPath = coverPath;
        }

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




