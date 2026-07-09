#include "category_dao.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

CategoryDAO::CategoryDAO(QSqlDatabase db) : m_db(db) {}

Category CategoryDAO::catFromQuery(QSqlQuery& query) const {
    Category c;
    c.id = query.value("id").toInt();
    c.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
    c.name = query.value("name").toString();
    c.path = query.value("path").toString();
    c.level = query.value("level").toInt();
    return c;
}

QVector<Category> CategoryDAO::getAll() const {
    QVector<Category> result;
    QSqlQuery query(m_db);
    query.exec("SELECT * FROM categories ORDER BY path");
    while (query.next()) result.append(catFromQuery(query));
    return result;
}

Category CategoryDAO::getById(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM categories WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) return catFromQuery(query);
    return Category{};
}

int CategoryDAO::insert(const Category& cat) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO categories (parent_id, name, path, level) VALUES (?, ?, ?, ?)");
    query.addBindValue(cat.parentId);
    query.addBindValue(cat.name);
    query.addBindValue(cat.path);
    query.addBindValue(cat.level);
    if (query.exec()) return query.lastInsertId().toInt();
    qWarning() << "Insert category failed:" << query.lastError().text();
    return -1;
}

bool CategoryDAO::update(const Category& cat) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE categories SET parent_id=?, name=?, path=?, level=? WHERE id=?");
    query.addBindValue(cat.parentId);
    query.addBindValue(cat.name);
    query.addBindValue(cat.path);
    query.addBindValue(cat.level);
    query.addBindValue(cat.id);
    return query.exec();
}

bool CategoryDAO::deleteCategory(int id) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM categories WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

int CategoryDAO::count() const {
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM categories");
    if (query.next()) return query.value(0).toInt();
    return 0;
}
