#ifndef MODELS_H
#define MODELS_H
#include<Qstring>
struct User {
    int id = 0;
    QString username;
    QString role = "reader";   // "admin" or "reader"
    // 其他字段后续添加
};

struct Book {
    int id;
    QString title;
    QString author;
};
#endif // MODELS_H
