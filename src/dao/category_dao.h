#ifndef CATEGORY_DAO_H
#define CATEGORY_DAO_H

#include "models.h"
#include <QVector>
#include <QSqlDatabase>

class CategoryDAO {
public:
    explicit CategoryDAO(QSqlDatabase db);
    QVector<Category> getAll() const;
    Category getById(int id) const;
    int insert(const Category& cat);
    bool update(const Category& cat);
    bool deleteCategory(int id);
    int count() const;
private:
    Category catFromQuery(class QSqlQuery& query) const;
    QSqlDatabase m_db;
};
#endif
