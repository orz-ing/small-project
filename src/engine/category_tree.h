#ifndef CATEGORY_TREE_H
#define CATEGORY_TREE_H

#include "models.h"
#include <QHash>
#include <QVector>

// 分类树：树形结构管理图书分类
class CategoryTree {
public:
    CategoryTree() = default;

    void build(const QVector<Category>& categories);

    // 获取子分类
    QVector<Category> getChildren(int parentId) const;

    // 获取所有后代分类ID（包括自身）
    QSet<int> getDescendantIds(int categoryId) const;

    // 获取完整路径
    QString getPath(int categoryId) const;

    // 获取根分类
    QVector<Category> getRootCategories() const;

    // 获取层级
    int getLevel(int categoryId) const;

    // 添加/更新/删除
    void addCategory(const Category& cat);
    void updateCategory(const Category& cat);
    void removeCategory(int categoryId);

    QVector<Category> allCategories() const { return m_categories.values(); }
    int size() const { return m_categories.size(); }

private:
    QHash<int, Category> m_categories;
    QHash<int, QVector<int>> m_children;  // parentId → childIds
};

#endif
