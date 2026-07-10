#ifndef API_BRIDGE_H
#define API_BRIDGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QSet>

#include "models.h"
#include "service/reservation_service.h"
#include "service/overdue_scanner.h"
#include "service/auth_service.h"
#include "service/book_service.h"
#include "service/borrow_service.h"
#include "service/log_service.h"
#include "engine/search_index.h"
#include "engine/recommend_engine.h"
#include "engine/category_tree.h"
#include "engine/credit_engine.h"
#include "dao/database_manager.h"

class ApiBridge : public QObject {
    Q_OBJECT
public:
    static ApiBridge* instance();

    bool initialize(const QString& dbPath = "library.db");
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Auth
    Result registerUser(const QString& username, const QString& password,
                        const QString& displayName = "", UserRole role = UserRole::Reader);
    Result login(const QString& username, const QString& password);
    User currentUser() const { return m_currentUser; }
    void logout();
    bool isAdmin() const { return m_currentUser.role == UserRole::Admin; }

    // Books
    QVector<Book> getAllBooks() const;
    Book getBookDetail(int bookId);
    Result addBook(const Book& book);
    Result updateBook(const Book& book);
    Result deleteBook(int bookId);
    QVector<Book> searchBooks(const QString& keywords, int topN = 10);
    QVector<Book> getBooksByCategory(int categoryId) const;

    // Borrow
    Result borrowBook(int userId, int bookId);
    Result returnBook(int recordId);
    Result renewBook(int recordId);
    QVector<BorrowRecord> getMyBorrowRecords(int userId) const;
    QVector<BorrowRecord> getMyActiveRecords(int userId) const;
    QVector<BorrowRecord> getAllBorrowRecords() const;
    int getBorrowCount(int bookId) const;

    // Reservation
    Result reserveBook(int userId, int bookId);
    Result cancelReservation(int reservationId);
    Result deleteReservationRecord(int reservationId);
    int deleteAllCancelledRecords();
    QVector<Reservation> getMyReservations(int userId) const;

    // Recommend
    QVector<Book> getRecommendations(int userId, int topN = 10);
    QVector<Book> getHotBooks(int topN = 10);
    void rebuildIndex();

    // Admin
    QVector<User> getAllUsers() const;
    Result updateUser(const User& user);
    Result deleteUser(int userId);
    bool checkUserBorrowing(int userId) const;
    Result disableUser(int userId, bool disabled);

    // Statistics
    Statistics getStatistics();

    // Categories
    QVector<Category> getAllCategories() const;
    QVector<Category> getCategoryChildren(int parentId) const;
    QVector<Category> getRootCategories() const;
    QSet<int> getDescendantCategoryIds(int categoryId) const;

    // Logs
    QVector<LogEntry> getRecentLogs() const;

    // Overdue scan
    void scanOverdue();

signals:
    void loggedIn(const User& user);
    void loggedOut();
    void bookAdded(const Book& book);
    void bookUpdated(const Book& book);
    void bookDeleted(int bookId);
    void borrowSucceeded(const QString& message);
    void returnSucceeded(const QString& message);
    void overdueFound(int userId, int recordId, double fine);

private:
    ApiBridge();
    ~ApiBridge() override;
    ApiBridge(const ApiBridge&) = delete;
    ApiBridge& operator=(const ApiBridge&) = delete;
    void initServices(const QSqlDatabase& db);

    bool m_initialized = false;
    User m_currentUser;
    SearchIndex* m_searchIndex = nullptr;
    CreditEngine* m_creditEngine = nullptr;
    RecommendEngine* m_recommendEngine = nullptr;
    CategoryTree* m_categoryTree = nullptr;
    AuthService* m_authService = nullptr;
    BookService* m_bookService = nullptr;
    BorrowService* m_borrowService = nullptr;
    ReservationService* m_reservationService = nullptr;
    OverdueScanner* m_overdueScanner = nullptr;
    LogService* m_logService = nullptr;
};

#endif