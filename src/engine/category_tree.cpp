#include "category_tree.h"

void CategoryTree::build(const QVector<Category>& categories) {
    m_categories.clear();
    m_children.clear();
    for (const auto& cat : categories) {
        addCategory(cat);
    }
}

void CategoryTree::addCategory(const Category& cat) {
    m_categories[cat.id] = cat;
    m_children[cat.parentId].append(cat.id);
}

void CategoryTree::updateCategory(const Category& cat) {
    // 如果父节点变了，需要更新父子关系
    auto oldIt = m_categories.find(cat.id);
    if (oldIt != m_categories.end() && oldIt->parentId != cat.parentId) {
        auto& oldChildren = m_children[oldIt->parentId];
        oldChildren.erase(std::remove(oldChildren.begin(), oldChildren.end(), cat.id), oldChildren.end());
        m_children[cat.parentId].append(cat.id);
    }
    m_categories[cat.id] = cat;
}

void CategoryTree::removeCategory(int categoryId) {
    auto cat = m_categories.find(categoryId);
    if (cat == m_categories.end()) return;

    // 从父节点的children列表中移除
    auto& siblings = m_children[cat->parentId];
    siblings.erase(std::remove(siblings.begin(), siblings.end(), categoryId), siblings.end());

    // 递归移除所有子分类
    QVector<int> children = m_children.value(categoryId);
    for (int childId : children) {
        removeCategory(childId);
    }

    m_categories.remove(categoryId);
    m_children.remove(categoryId);
}

QVector<Category> CategoryTree::getChildren(int parentId) const {
    QVector<Category> result;
    auto childIds = m_children.value(parentId);
    for (int id : childIds) {
        auto it = m_categories.find(id);
        if (it != m_categories.end()) result.append(it.value());
    }
    return result;
}

QSet<int> CategoryTree::getDescendantIds(int categoryId) const {
    QSet<int> result;
    result.insert(categoryId);
    auto childIds = m_children.value(categoryId);
    for (int id : childIds) {
        result.unite(getDescendantIds(id));
    }
    return result;
}

QString CategoryTree::getPath(int categoryId) const {
    auto it = m_categories.find(categoryId);
    if (it == m_categories.end()) return "";
    return it->path;
}

int CategoryTree::getLevel(int categoryId) const {
    auto it = m_categories.find(categoryId);
    if (it == m_categories.end()) return -1;
    return it->level;
}

QVector<Category> CategoryTree::getRootCategories() const {
    return getChildren(-1);
}
