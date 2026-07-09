#ifndef API_BRIDGE_H
#define API_BRIDGE_H

#include "models.h"
#include "service/auth_service.h"
#include "service/book_service.h"
#include "service/borrow_service.h"
#include "service/reservation_service.h"
#include "service/overdue_scanner.h"
#include "service/log_service.h"
#include "engine/search_index.h"
#include "engine/recommend_engine.h"
#include "engine/category_tree.h"
#include "engine/credit_engine.h"
#include "dao/database_manager.h"
#include <QObject>
#include <QSet>

// API桥接层：前端唯一入口，单例模式
class ApiBridge : public QObject {
    Q_OBJECT
public:
    static ApiBridge* instance();

    bool initialize(const QString& dbPath = "library.db");
    void shutdown();

    // ============ 认证 ============
    Result login(const QString& username, const QString& password);
    Result registerUser(const QString& username, const QString& password,
                        const QString& displayName, UserRole role = UserRole::Reader);
    void logout();
    User currentUser() const { return m_authService ? m_authService->currentUser() : User{}; }
    bool isLoggedIn() const { return m_authService && m_authService->isLoggedIn(); }
    bool isAdmin() const { return m_authService && m_authService->isAdmin(); }

    // ============ 检索 ============
    QVector<Book> searchBooks(const QString& keywords, int topN = 20);
    Book getBookDetail(int bookId);
    QVector<Book> getRecommendations(int userId, int topN = 10);
    QVector<Book> getHotBooks(int topN = 10);

    // ============ 借阅 ============
    Result borrowBook(int userId, int bookId);
    Result returnBook(int recordId);
    Result renewBook(int recordId);
    QVector<BorrowRecord> getMyBorrowRecords(int userId) const;
    QVector<BorrowRecord> getMyActiveRecords(int userId) const;

    // ============ 预约 ============
    Result reserveBook(int userId, int bookId);
    Result cancelReservation(int reservationId);
    QVector<Reservation> getMyReservations(int userId) const;

    // ============ 管理 ============
    Result addBook(const Book& book);
    Result updateBook(const Book& book);
    Result deleteBook(int bookId);
    QVector<Book> getAllBooks() const;
    QVector<User> getAllUsers() const;
    Result disableUser(int userId, bool disabled);

    // ============ 统计 ============
    Statistics getStatistics();
    QVector<Category> getAllCategories() const;

    // ============ 分类 ============
    QVector<Category> getCategoryChildren(int parentId) const;
    QVector<Category> getRootCategories() const;
    QSet<int> getDescendantCategoryIds(int categoryId) const;

    // ============ 日志 ============
    QVector<LogEntry> getRecentLogs() const;

    // ============ 数据刷新 ============
    void rebuildIndex();

signals:
    void loggedIn(const User& user);
    void loggedOut();
    void borrowSucceeded(const QString& message);
    void returnSucceeded(const QString& message);
    void bookAdded(const Book& book);
    void bookUpdated(const Book& book);
    void bookDeleted(int bookId);
    void overdueFound(int userId, int recordId, double fine);

private:
    ApiBridge();
    ~ApiBridge() override;
    ApiBridge(const ApiBridge&) = delete;
    ApiBridge& operator=(const ApiBridge&) = delete;

    AuthService* m_authService = nullptr;
    BookService* m_bookService = nullptr;
    BorrowService* m_borrowService = nullptr;
    ReservationService* m_reservationService = nullptr;
    OverdueScanner* m_overdueScanner = nullptr;
    LogService* m_logService = nullptr;
    SearchIndex* m_searchIndex = nullptr;
    RecommendEngine* m_recommendEngine = nullptr;
    CategoryTree* m_categoryTree = nullptr;
    CreditEngine* m_creditEngine = nullptr;
    bool m_initialized = false;
};

#endif // API_BRIDGE_H

