// category_tree.cpp — 分类树
#include "backend.h"
using namespace std;

// ===== 构建树 =====
// 输入: 扁平分类列表（所有 Category 对象）
void CategoryTree::buildTree(const vector<Category>& categories)
{
    // TODO: 遍历 → 建立 parentId → children 映射 → 填充 nodes_
}

// ===== 获取直接子分类 =====
// 输入: 分类 ID
// 输出: 该分类的所有直接子分类 ID 列表
vector<int> CategoryTree::getChildren(int categoryId) const
{
    // TODO: 从 nodes_[categoryId].children 返回
}

// ===== 获取所有子孙分类 =====
// 输入: 分类 ID
// 输出: 该分类下全部子孙分类 ID（递归展开）
vector<int> CategoryTree::getAllDescendants(int categoryId) const
{
    // TODO: DFS/BFS 遍历子树
}

// ===== 获取分类路径 =====
// 输入: 分类 ID
// 输出: 从根到该分类的路径字符串（如 "计算机/编程语言/C++"）
string CategoryTree::getPath(int categoryId) const
{
    // TODO: 回溯 parentId 直到根，拼接路径
}

// ===== 按名称查找 =====
// 输入: 分类名称
// 输出: 匹配的分类 ID; -1=未找到
int CategoryTree::findByName(const string& name) const
{
    // TODO: 遍历 nodes_ 匹配名称
}
