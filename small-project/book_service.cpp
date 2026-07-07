#include "backend.h"
#include "utils.h"

BookService::BookService(BookDao* bDao, LogService* lSvc, SearchIndex* idx, RecommendEngine* rec, CategoryTree* cat)
    : bookDao_(bDao), logSvc_(lSvc), searchIdx_(idx), recommendEng_(rec), categoryTree_(cat) {}

pair<vector<Book>, int> BookService::search(const string& query, int categoryId,
    int page, int pageSize, Status& status, bool& done) {
    done = false;
    auto allBooks = bookDao_->findAll();
    
    // Use SearchIndex if query is not empty
    vector<Book> scored;
    if (!query.empty()) {
        auto results = searchIdx_->search(query, (int)allBooks.size());
        unordered_map<int, double> scoreMap;
        for (auto& r : results) scoreMap[r.bookId] = r.score;
        
        for (auto& b : allBooks) {
            auto it = scoreMap.find(b.getId());
            if (it != scoreMap.end()) {
                if (categoryId <= 0 || b.getCategoryId() == categoryId)
                    scored.push_back(b);
            }
        }
    } else {
        for (auto& b : allBooks) {
            if (categoryId <= 0 || b.getCategoryId() == categoryId)
                scored.push_back(b);
        }
    }
    
    int total = (int)scored.size();
    int start = (page - 1) * pageSize;
    vector<Book> pageResult;
    for (int i = start; i < min(start + pageSize, total); i++)
        pageResult.push_back(scored[i]);
    
    done = true; status = StatusOK();
    return {pageResult, total};
}

Book BookService::getDetail(int bookId) {
    return bookDao_->findById(bookId);
}

vector<Book> BookService::getRecommendations(int userId, int topN, Status& status, bool& done) {
    done = false;
    // Simple content-based: return random popular books as recommendations
    auto allBooks = bookDao_->findAll();
    vector<Book> result;
    int n = min(topN, (int)allBooks.size());
    for (int i = 0; i < n; i++)
        result.push_back(allBooks[i]);
    done = true; status = StatusOK();
    return result;
}

Status BookService::addBook(const Book& book, int adminId) {
    Status s; bookDao_->insert(book, s);
    if (s.ok()) {
        if (searchIdx_) searchIdx_->addBook(book);
        if (logSvc_) logSvc_->log(adminId, "add_book", book.getTitle(), "isbn="+book.getIsbn());
    }
    return s;
}

Status BookService::updateBook(const Book& book, int adminId) {
    auto s = bookDao_->update(book);
    if (s.ok() && logSvc_) logSvc_->log(adminId, "update_book", book.getTitle(), "id="+to_string(book.getId()));
    return s;
}

Status BookService::deleteBook(int bookId, int adminId) {
    auto book = bookDao_->findById(bookId);
    auto s = bookDao_->deleteBook(bookId);
    if (s.ok()) {
        if (searchIdx_) searchIdx_->removeBook(bookId);
        if (logSvc_) logSvc_->log(adminId, "delete_book", book.getTitle(), "id="+to_string(bookId));
    }
    return s;
}
