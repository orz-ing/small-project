#include "backend.h"
#include <algorithm>

void CategoryTree::buildTree(const vector<Category>& categories) {
    nodes_.clear();
    rootId_ = 0;
    for (auto& c : categories) {
        Node n;
        n.id = c.getId();
        n.parentId = c.getParentId();
        n.name = c.getName();
        n.level = c.getLevel();
        nodes_[n.id] = n;
        if (c.getParentId() == 0) rootId_ = c.getId();
    }
    // Build children lists
    for (auto& [id, node] : nodes_) {
        if (node.parentId > 0 && nodes_.find(node.parentId) != nodes_.end())
            nodes_[node.parentId].children.push_back(id);
    }
}

vector<int> CategoryTree::getChildren(int categoryId) const {
    auto it = nodes_.find(categoryId);
    if (it == nodes_.end()) return {};
    return it->second.children;
}

vector<int> CategoryTree::getAllDescendants(int categoryId) const {
    vector<int> result;
    auto it = nodes_.find(categoryId);
    if (it == nodes_.end()) return result;
    for (int childId : it->second.children) {
        result.push_back(childId);
        auto sub = getAllDescendants(childId);
        result.insert(result.end(), sub.begin(), sub.end());
    }
    return result;
}

string CategoryTree::getPath(int categoryId) const {
    string path;
    int cur = categoryId;
    while (cur > 0) {
        auto it = nodes_.find(cur);
        if (it == nodes_.end()) break;
        path = path.empty() ? it->second.name : it->second.name + " / " + path;
        cur = it->second.parentId;
    }
    return path;
}

int CategoryTree::findByName(const string& name) const {
    for (auto& [id, node] : nodes_) {
        if (node.name == name) return id;
    }
    return -1;
}
