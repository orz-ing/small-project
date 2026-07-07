#include "backend.h"
#include <sqlite3.h>
CategoryDao::CategoryDao(DatabaseManager& db) : db_(db) {}
vector<Category> CategoryDao::findAll() {
    auto* db=(sqlite3*)db_.getHandle(); vector<Category> cats;
    const char* sql="SELECT * FROM categories ORDER BY level,id";
    sqlite3_stmt* stmt=nullptr; if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return cats;
    while(sqlite3_step(stmt)==SQLITE_ROW){
        Category c; c.setId(sqlite3_column_int(stmt,0)); c.setName((const char*)sqlite3_column_text(stmt,1));
        c.setParentId(sqlite3_column_int(stmt,2)); c.setLevel(sqlite3_column_int(stmt,3)); cats.push_back(c);
    }
    sqlite3_finalize(stmt); return cats;
}
Category CategoryDao::insert(const Category& c, Status& status) {
    auto* db=(sqlite3*)db_.getHandle();
    const char* sql="INSERT INTO categories (name,parent_id,level) VALUES (?,?,?)";
    sqlite3_stmt* stmt=nullptr;
    if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK){status={StatusCode::ERR_DB_EXEC,"prepare failed"};return Category();}
    sqlite3_bind_text(stmt,1,c.getName().c_str(),-1,SQLITE_STATIC); sqlite3_bind_int(stmt,2,c.getParentId()); sqlite3_bind_int(stmt,3,c.getLevel());
    if(sqlite3_step(stmt)!=SQLITE_DONE){status={StatusCode::ERR_DB_EXEC,"insert failed"};sqlite3_finalize(stmt);return Category();}
    int id=(int)sqlite3_last_insert_rowid(db); sqlite3_finalize(stmt);
    Category created=c; created.setId(id); status=StatusOK(); return created;
}
Status CategoryDao::update(const Category& c){
    return db_.execute("UPDATE categories SET name='"+c.getName()+"',parent_id="+to_string(c.getParentId())+",level="+to_string(c.getLevel())+" WHERE id="+to_string(c.getId()));
}
Status CategoryDao::deleteCategory(int categoryId){
    db_.execute("UPDATE books SET category_id=0 WHERE category_id="+to_string(categoryId));
    db_.execute("DELETE FROM categories WHERE parent_id="+to_string(categoryId));
    return db_.execute("DELETE FROM categories WHERE id="+to_string(categoryId));
}
