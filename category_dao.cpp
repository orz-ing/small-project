// category_dao.cpp — 分类数据访问对象
#include "backend.h"
using namespace std;

CategoryDao::CategoryDao(DatabaseManager& db) : db_(db) {}

// 输出: 所有分类列表
vector<Category> CategoryDao::findAll()
{
    // TODO: SELECT * FROM categories ORDER BY level, id
}

// 输入: Category 对象
// 输出: 新插入的 id
int CategoryDao::insert(const Category& c)
{
    // TODO: INSERT INTO categories
}

bool CategoryDao::update(const Category& c)
{
    // TODO: UPDATE categories SET ... WHERE id=?
}

// 输入: categoryId
// 输出: true=删除成功; false=有子分类，禁止删除
bool CategoryDao::deleteCategory(int categoryId)
{
    // TODO: 先检查是否有子分类 → 有则返回 false → 否则 DELETE
}
