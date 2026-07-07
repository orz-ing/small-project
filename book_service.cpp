// book_service.cpp — 图书服务
#include "backend.h"
using namespace std;

BookService::BookService(BookDao* bDao, LogService* lSvc,
    SearchIndex* idx, RecommendEngine* rec, CategoryTree* cat)
    : bookDao_(bDao), logSvc_(lSvc), searchIdx_(idx),
      recommendEng_(rec), categoryTree_(cat) {}

// 输入: 关键词, 分类ID(-1=全部), 页码, 每页条数
// 输出: (图书列表, 总条数)
pair<vector<Book>, int> BookService::search(const string& query, int categoryId,
    int page, int pageSize)
{
    // TODO: searchIdx_->search(query) → 按分类过滤 → 分页 → 通过 bookDao 获取完整 Book
}

// 输入: bookId
// 输出: Book 对象
Book BookService::getDetail(int bookId)
{
    // TODO: bookDao_->findById(bookId)
}

// 输入: userId, 推荐数量
// 输出: 推荐图书列表（已排除该用户借过的书）
vector<Book> BookService::getRecommendations(int userId, int topN)
{
    // TODO: recommendEng_->recommend(...) → bookDao_->findByIds(...)
}

// 输入: Book 对象, 操作的管理员ID
// 输出: true=添加成功
bool BookService::addBook(const Book& book, int adminId)
{
    // TODO: bookDao_->insert(book) → searchIdx_->addBook() → logSvc_->log()
}

// 输入: Book 对象, 操作的管理员ID
// 输出: true=更新成功
bool BookService::updateBook(const Book& book, int adminId)
{
    // TODO: bookDao_->update(book) → searchIdx_ 同步更新 → logSvc_->log()
}

// 输入: bookId, 操作的管理员ID
// 输出: true=删除成功（前提：无未还借阅）
bool BookService::deleteBook(int bookId, int adminId)
{
    // TODO: 检查未还借阅 → bookDao_->deleteBook() → searchIdx_->removeBook() → log
}
