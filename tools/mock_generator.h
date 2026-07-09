#ifndef MOCK_GENERATOR_H
#define MOCK_GENERATOR_H

#include "../src/models.h"
#include <QVector>
#include <QString>
#include <QSqlDatabase>

class MockGenerator {
public:
    // bookCount, userCount, borrowCount, reservationCount, categoryCount
    static void generate(int bookCount, int userCount, int borrowCount,
                         int reservationCount, int categoryCount);

    // 生成并写入数据库
    static void generateToDB(QSqlDatabase db, int bookCount = 30, int userCount = 8,
                             int borrowCount = 25, int reservationCount = 8, int categoryCount = 10);

    // 生成分类
    static QVector<Category> generateCategories(int count);
    // 生成图书
    static QVector<Book> generateBooks(int count, const QVector<Category>& categories);
    // 生成用户
    static QVector<User> generateUsers(int count);
    // 生成借阅记录
    static QVector<BorrowRecord> generateBorrowRecords(int count,
        const QVector<User>& users, const QVector<Book>& books);
    // 生成预约
    static QVector<Reservation> generateReservations(int count,
        const QVector<User>& users, const QVector<Book>& books);
};

#endif
