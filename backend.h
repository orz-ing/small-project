/******************************************************************************
 * backend.h — 图书管理系统后端统一头文件（重构版 v2.0）
 *
 * 架构层次（自上而下）：
 *   ApiBridge（单例，前端唯一入口）
 *       ↓
 *   Service 层（事务控制 + 并发 + 定时任务）
 *       ↓
 *   DAO 层（数据库 CRUD + SQLite）
 *       ↔
 *   Algorithm 层（纯函数，无状态，独立可测）
 ******************************************************************************/

#ifndef BACKEND_H
#define BACKEND_H

// ========== C++ 标准库 ==========
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <memory>
#include <algorithm>
#include <mutex>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;


// ╔══════════════════════════════════════════════════════════════╗
// ║  全局状态码体系（所有可失败操作的统一返回值）                      ║
// ╚══════════════════════════════════════════════════════════════╝

enum class StatusCode {
    OK = 0,
    ERR_DB_OPEN,               // 数据库打开失败
    ERR_DB_EXEC,               // SQL 执行失败
    ERR_DB_TRANSACTION,        // 事务失败
    ERR_USER_NOT_FOUND,        // 用户不存在
    ERR_USER_DISABLED,         // 用户被禁用
    ERR_USER_EXISTS,           // 用户名已存在
    ERR_WRONG_PASSWORD,        // 密码错误
    ERR_BOOK_NOT_FOUND,        // 图书不存在
    ERR_BOOK_OUT_OF_STOCK,     // 库存不足
    ERR_BOOK_HAS_BORROW,       // 有未还借阅
    ERR_BORROW_NOT_FOUND,      // 借阅记录不存在
    ERR_RENEW_LIMIT,           // 续借已达上限
    ERR_RENEW_OVERDUE,         // 已逾期不能续借
    ERR_CREDIT_TOO_LOW,        // 信用分不足
    ERR_VERSION_CONFLICT,      // 乐观锁冲突
    ERR_RESERVATION_EXISTS,    // 已有预约
    ERR_RESERVATION_NOT_FOUND, // 预约不存在
    ERR_RESERVATION_EXPIRED,   // 预约已过期
    ERR_CATEGORY_HAS_CHILD,    // 有子分类
    ERR_CATEGORY_NOT_FOUND,    // 分类不存在
    ERR_PERMISSION_DENIED,     // 权限不足
    ERR_FILE_OPEN,             // 文件打开失败
    ERR_FILE_WRITE,            // 文件写入失败
    ERR_UNKNOWN                // 未知错误
};

// Status 状态包 — 调用方可 if (!status) 判断并输出 status.msg 给用户
struct Status {
    StatusCode code = StatusCode::OK;
    string msg;
    Status() = default;
    Status(StatusCode c, string m = "") : code(c), msg(m) {}
    bool ok()            const { return code == StatusCode::OK; }
    operator bool()      const { return code == StatusCode::OK; }
    string getCodeName() const;   // 返回状态码英文名（用于日志）
};
inline Status StatusOK()                                  { return {StatusCode::OK, ""}; }
inline Status StatusErr(StatusCode c, const string& m)    { return {c, m}; }

// ================================================================
// 【运行状态标识】
// 对于可能耗时较长的操作（搜索、借阅事务、批量导入等），
// 调用方传入一个 bool& done 引用变量，由被调用函数维护：
//
//   done = false  →  操作正在执行中（调用方可轮询此变量）
//   done = true   →  操作已结束（成功或失败，配合 Status 判断结果）
//
// 示例：
//   bool done = false;
//   auto result = apiBridge.searchBooks("C++", -1, 1, 20, status, done);
//   while (!done) { QApplication::processEvents(); }  // 前端保持响应
//   if (status.ok()) { /* 使用 result */ }
// ================================================================


// ╔══════════════════════════════════════════════════════════════╗
// ║  第一部分：数据模型（Model）                                  ║
// ║  所有数据字段为 private，通过 public 方法访问                   ║
// ╚══════════════════════════════════════════════════════════════╝

// ----------------------------------------------------------------
//  M1. User — 用户基类
//      派生：Reader（读者）/ Admin（管理员）
// ----------------------------------------------------------------
class User {
private:
    int    id_;
    string username_;          // 唯一，登录账号
    string passwordHash_;      // SHA-256 哈希，禁止明文
    string realName_;          // 真实姓名
    string role_;              // "reader" | "admin"
    int    credit_;            // 信用分，范围 [0, 100]，初始 100
    string status_;            // "active" | "disabled"
    string phone_;
    string email_;
    string createTime_;        // "YYYY-MM-DD HH:MM:SS"

public:
    // —— 构造 ——
    User();
    User(const string& username, const string& passwordHash,
         const string& realName, const string& role);

    // —— Getter（只读访问） ——
    int    getId()            const { return id_; }
    string getUsername()      const { return username_; }
    string getPasswordHash()  const { return passwordHash_; }
    string getRealName()      const { return realName_; }
    string getRole()          const { return role_; }
    int    getCredit()        const { return credit_; }
    string getStatus()        const { return status_; }
    string getPhone()         const { return phone_; }
    string getEmail()         const { return email_; }
    string getCreateTime()    const { return createTime_; }

    // —— Setter（受控修改） ——
    void setId(int id)                  { id_ = id; }
    void setPasswordHash(const string& h) { passwordHash_ = h; }
    void setRealName(const string& n)     { realName_ = n; }
    void setPhone(const string& p)        { phone_ = p; }
    void setEmail(const string& e)        { email_ = e; }
    void setCreateTime(const string& t)   { createTime_ = t; }

    // —— 信用分操作 ——
    // 输入: delta（正数加/负数扣）；输出: 操作后的信用分（自动 clamp 到 [0,100]）
    int  adjustCredit(int delta);
    void setCredit(int c)                { credit_ = max(0, min(100, c)); }

    // —— 状态操作 ——
    // 输入: status("active"/"disabled")；输出: 是否设置成功
    bool setStatus(const string& s);
    bool isActive() const { return status_ == "active"; }
    bool isAdmin()  const { return role_ == "admin"; }

    // —— 权限判断 ——
    // 输出: 信用分 ≥ 60 且账号未禁用 → true
    bool canBorrow() const;
};


// ----------------------------------------------------------------
//  M2. Admin — 管理员（继承 User）
//      拥有额外的管理权限
// ----------------------------------------------------------------
class Admin : public User {
private:
    int    adminLevel_;        // 权限级别: 1=普通管理员, 2=超级管理员
    string department_;        // 所属部门

public:
    Admin();
    Admin(const string& username, const string& passwordHash,
          const string& realName, int level = 1);

    // —— Getter ——
    int    getAdminLevel() const { return adminLevel_; }
    string getDepartment()  const { return department_; }

    // —— Setter ——
    void setAdminLevel(int l)   { adminLevel_ = l; }
    void setDepartment(const string& d) { department_ = d; }

    // —— 权限检查 ——
    // 输出: level>=2 则拥有删除图书/禁用用户等敏感操作权限
    bool canDeleteBook()   const { return adminLevel_ >= 2; }
    bool canDisableUser()  const { return adminLevel_ >= 2; }
    bool canViewAllLogs()  const { return adminLevel_ >= 2; }
};


// ----------------------------------------------------------------
//  M3. Book — 图书类
//      封装图书库存的增减逻辑，确保数据一致性
// ----------------------------------------------------------------
class Book {
private:
    int    id_;
    string isbn_;
    string title_;
    string author_;
    string publisher_;
    string publishDate_;
    int    categoryId_;          // 分类 ID，关联 Category
    int    stock_;               // 当前可借库存（≥ 0）
    int    totalStock_;          // 总复本数（≥ stock）
    int    version_;             // 乐观锁版本号
    string location_;            // 书架位置
    string description_;         // 简介
    string coverUrl_;            // 封面图片 URL

public:
    Book();

    // —— Getter ——
    int    getId()          const { return id_; }
    string getIsbn()        const { return isbn_; }
    string getTitle()       const { return title_; }
    string getAuthor()      const { return author_; }
    string getPublisher()   const { return publisher_; }
    string getPublishDate() const { return publishDate_; }
    int    getCategoryId()  const { return categoryId_; }
    int    getStock()       const { return stock_; }
    int    getTotalStock()  const { return totalStock_; }
    int    getVersion()     const { return version_; }
    string getLocation()    const { return location_; }
    string getDescription() const { return description_; }
    string getCoverUrl()    const { return coverUrl_; }

    // —— Setter ——
    void setId(int id)                { id_ = id; }
    void setIsbn(const string& s)     { isbn_ = s; }
    void setTitle(const string& t)    { title_ = t; }
    void setAuthor(const string& a)   { author_ = a; }
    void setPublisher(const string& p){ publisher_ = p; }
    void setPublishDate(const string& d) { publishDate_ = d; }
    void setCategoryId(int cid)       { categoryId_ = cid; }
    void setTotalStock(int ts)        { totalStock_ = ts; }
    void setVersion(int v)            { version_ = v; }
    void setLocation(const string& l) { location_ = l; }
    void setDescription(const string& d) { description_ = d; }
    void setCoverUrl(const string& u) { coverUrl_ = u; }
    void setStock(int s)              { stock_ = s; }

    // —— 库存操作（核心） ——
    // 输入: delta（+1归还, -1借出）；输出: 操作后的库存量（失败返回 -1）
    int  adjustStock(int delta);

    // 输出: stock > 0 则可借
    bool isAvailable()   const;

    // 乐观锁版本号递增
    void bumpVersion()   { version_++; }
};


// ----------------------------------------------------------------
//  M4. BorrowRecord — 借阅记录
// ----------------------------------------------------------------
class BorrowRecord {
private:
    int    id_;
    int    userId_;
    int    bookId_;
    string borrowDate_;       // "YYYY-MM-DD"
    string dueDate_;          // 应还日期 = borrowDate + 30天
    string returnDate_;       // 空串 = 未归还
    string status_;           // "borrowing" | "returned" | "overdue"
    int    renewCount_;       // 续借次数，上限 2

public:
    BorrowRecord();

    // —— Getter ——
    int    getId()         const { return id_; }
    int    getUserId()     const { return userId_; }
    int    getBookId()     const { return bookId_; }
    string getBorrowDate() const { return borrowDate_; }
    string getDueDate()    const { return dueDate_; }
    string getReturnDate() const { return returnDate_; }
    string getStatus()     const { return status_; }
    int    getRenewCount() const { return renewCount_; }

    // —— Setter ——
    void setId(int i)                { id_ = i; }
    void setUserId(int u)            { userId_ = u; }
    void setBookId(int b)            { bookId_ = b; }
    void setBorrowDate(const string& d) { borrowDate_ = d; }
    void setDueDate(const string& d)    { dueDate_ = d; }
    void setReturnDate(const string& d) { returnDate_ = d; }
    void setRenewCount(int r)           { renewCount_ = r; }

    // —— 状态操作 ——
    // 输入: status("borrowing"/"returned"/"overdue"); 输出: 是否合法
    bool setStatus(const string& s);

    // 输入: 当前日期("YYYY-MM-DD"); 输出: 是否已逾期
    bool isOverdue(const string& today) const;

    // 输入: 续借延长天数(默认15)；输出: 新的应还日期（已达上限返回空串）
    string renew(int extraDays = 15);
};


// ----------------------------------------------------------------
//  M5. Reservation — 预约记录
// ----------------------------------------------------------------
class Reservation {
private:
    int    id_;
    int    userId_;
    int    bookId_;
    string reserveDate_;      // 预约时间
    string expireDate_;       // 过期时间（+48h）
    string status_;           // "pending" | "fulfilled" | "cancelled" | "expired"
    int    priority_;         // 0=普通, 1=优先(信用≥90)

public:
    Reservation();

    // —— Getter ——
    int    getId()          const { return id_; }
    int    getUserId()      const { return userId_; }
    int    getBookId()      const { return bookId_; }
    string getReserveDate() const { return reserveDate_; }
    string getExpireDate()  const { return expireDate_; }
    string getStatus()      const { return status_; }
    int    getPriority()    const { return priority_; }

    // —— Setter ——
    void setId(int i)               { id_ = i; }
    void setUserId(int u)           { userId_ = u; }
    void setBookId(int b)           { bookId_ = b; }
    void setReserveDate(const string& d) { reserveDate_ = d; }
    void setExpireDate(const string& d)  { expireDate_ = d; }
    void setPriority(int p)             { priority_ = p; }

    // —— 状态操作 ——
    bool setStatus(const string& s);

    // 输入: 当前时间; 输出: 是否已过期
    bool isExpired(const string& now) const;
};


// ----------------------------------------------------------------
//  M6. Category — 图书分类
// ----------------------------------------------------------------
class Category {
private:
    int    id_;
    string name_;
    int    parentId_;         // 父分类 ID，0 表示根分类
    int    level_;            // 层级深度（根为 0）

public:
    Category();

    int    getId()       const { return id_; }
    string getName()     const { return name_; }
    int    getParentId() const { return parentId_; }
    int    getLevel()    const { return level_; }

    void setId(int i)              { id_ = i; }
    void setName(const string& n)  { name_ = n; }
    void setParentId(int p)        { parentId_ = p; }
    void setLevel(int l)           { level_ = l; }

    // 输出: parentId==0 则为根分类
    bool isRoot() const { return parentId_ == 0; }
};


// ----------------------------------------------------------------
//  M7. LogEntry — 日志记录
// ----------------------------------------------------------------
class LogEntry {
private:
    int    id_;
    int    adminId_;
    string action_;          // 操作类型，如 "delete_book"
    string target_;          // 操作对象描述
    string detail_;          // 详情
    string time_;            // 时间戳

public:
    LogEntry();

    int    getId()      const { return id_; }
    int    getAdminId() const { return adminId_; }
    string getAction()  const { return action_; }
    string getTarget()  const { return target_; }
    string getDetail()  const { return detail_; }
    string getTime()    const { return time_; }

    void setId(int i)             { id_ = i; }
    void setAdminId(int a)        { adminId_ = a; }
    void setAction(const string& a) { action_ = a; }
    void setTarget(const string& t) { target_ = t; }
    void setDetail(const string& d) { detail_ = d; }
    void setTime(const string& t)   { time_ = t; }
};


// ╔══════════════════════════════════════════════════════════════╗
// ║  第二部分：算法引擎（Algorithm） — 王博涵                       ║
// ║  纯函数，无状态，无 DB 依赖，可独立单元测试                       ║
// ╚══════════════════════════════════════════════════════════════╝

// ----------------------------------------------------------------
//  A1. CreditEngine — 信用积分引擎
// ----------------------------------------------------------------
class CreditEngine {
public:
    static constexpr int INITIAL_SCORE    = 100;
    static constexpr int BORROW_THRESHOLD = 60;    // 低于此分禁止借阅
    static constexpr int MAX_SCORE        = 100;
    static constexpr int PENALTY_OVERDUE  = -10;   // 逾期归还扣分
    static constexpr int PENALTY_DAMAGE   = -20;   // 损坏扣分
    static constexpr int PENALTY_SEVERE   = -30;   // 严重逾期(>30天)
    static constexpr int REWARD_ON_TIME   = +5;    // 按时归还加分
    static constexpr int REWARD_DONATE    = +10;   // 捐赠加分

    // 输入: 当前信用分, 操作类型("overdue"/"on_time"/"damage"/"donate")
    // 输出: 新的信用分（自动 clamp 到 [0, MAX_SCORE]）
    static int updateScore(int currentScore, const string& action);

    // 输入: 信用分; 输出: 是否可借阅
    static bool canBorrow(int score);

    // 输入: 信用分; 输出: "优秀"(≥90) | "良好"(≥75) | "一般"(≥60) | "较差"(<60)
    static string getLevel(int score);
};


// ----------------------------------------------------------------
//  A2. FineCalculator — 阶梯费率罚款计算器
// ----------------------------------------------------------------
class FineCalculator {
private:
    static constexpr double RATE_TIER1  = 0.5;    // 1-7天
    static constexpr double RATE_TIER2  = 1.0;    // 8-30天
    static constexpr double RATE_TIER3  = 2.0;    // >30天
    static constexpr int    TIER1_DAYS  = 7;
    static constexpr int    TIER2_DAYS  = 30;
    static constexpr double MAX_FINE    = 100.0;

public:
    // 输入: dueDate(应还日期"YYYY-MM-DD"), returnDate(实际归还日期)
    // 输出: 罚款金额（元），0 表示未逾期
    static double calcFine(const string& dueDate, const string& returnDate);
};


// ----------------------------------------------------------------
//  A3. SearchIndex — 倒排索引搜索引擎
// ----------------------------------------------------------------
struct SearchResult {
    int    bookId;
    double score;        // 相关性得分（越高越相关）
};

class SearchIndex {
private:
    // 核心: 关键词 → bookId 集合
    unordered_map<string, set<int>> invertedIndex_;
    // 辅助: bookId → 借阅次数（热度排序用）
    unordered_map<int, int> borrowCount_;

    // 分词: 输入文本 → 输出关键词列表
    vector<string> tokenize(const string& text) const;

public:
    // 输入: 全部图书列表; 输出: 无（构建索引）
    void buildIndex(const vector<Book>& books);

    // 输入: 搜索关键词, 返回数量; 输出: 按相关性降序的搜索结果
    vector<SearchResult> search(const string& query, int topN = 20) const;

    // 增量更新
    void addBook(const Book& book);
    void removeBook(int bookId);
};


// ----------------------------------------------------------------
//  A4. RecommendEngine — 协同过滤推荐引擎
// ----------------------------------------------------------------
class RecommendEngine {
private:
    // userId → 借过的 bookId 集合
    unordered_map<int, set<int>> userBooks_;
    // (bookA, bookB) → 同时被借的次数
    map<pair<int,int>, int> cooccurrence_;

    // 输入: 两个集合; 输出: Jaccard 相似度 [0,1]
    double jaccardSimilarity(const set<int>& a, const set<int>& b) const;

public:
    // 输入: 所有借阅记录; 输出: 无（构建共现矩阵）
    void buildMatrix(const vector<BorrowRecord>& records);

    // 输入: 目标用户ID, 推荐数量, 已借记录, 全部图书列表
    // 输出: 推荐图书ID列表（按推荐分数降序）
    vector<int> recommend(int targetUserId, int topN,
                          const vector<BorrowRecord>& userHistory,
                          const vector<Book>& allBooks) const;

    // 实时更新: 有新借阅时调用
    void onBorrow(int userId, int bookId);
};


// ----------------------------------------------------------------
//  A5. CategoryTree — 分类树
// ----------------------------------------------------------------
class CategoryTree {
private:
    struct Node {
        int id;
        int parentId;
        string name;
        int level;
        vector<int> children;
    };
    unordered_map<int, Node> nodes_;
    int rootId_ = 0;

public:
    // 输入: 扁平分类列表; 输出: 无（构建树）
    void buildTree(const vector<Category>& categories);

    // 输入: 分类ID; 输出: 直接子分类ID列表
    vector<int> getChildren(int categoryId) const;

    // 输入: 分类ID; 输出: 所有子孙分类ID（递归展开）
    vector<int> getAllDescendants(int categoryId) const;

    // 输入: 分类ID; 输出: 从根到该分类的路径（如 "计算机/编程语言/C++"）
    string getPath(int categoryId) const;

    // 输入: 分类名; 输出: 分类ID（-1 未找到）
    int findByName(const string& name) const;
};


// ----------------------------------------------------------------
//  A6. ReservationQueue — FIFO 预约队列（线程安全）
// ----------------------------------------------------------------
class ReservationQueue {
private:
    struct QueueItem {
        int userId;
        int priority;      // 0=普通, 1=优先
        time_t timestamp;
    };
    // bookId → 等待队列
    unordered_map<int, queue<QueueItem>> queues_;
    mutable mutex mtx_;

public:
    // 输入: bookId, userId, priority(0/1)
    // 输出: 排队位置（从 1 开始），-1 表示已在队列中
    int  enqueue(int bookId, int userId, int priority = 0);

    // 输入: bookId; 输出: 队首 userId（-1 表示队列空）
    int  dequeue(int bookId);

    // 输入: bookId, userId
    void cancel(int bookId, int userId);

    // 输入: bookId, userId; 输出: 排队位置（-1 不在队列）
    int  getPosition(int bookId, int userId) const;

    // 输入: 超时小时数(默认48); 输出: (bookId, userId) 超时预约列表
    vector<pair<int,int>> processExpired(int timeoutHours = 48);

    // 输入: bookId; 输出: 队列长度
    int  size(int bookId) const;

    // 输入: bookId; 输出: 查看队首（不弹出，-1 表示空）
    int  peekFront(int bookId) const;
};


// ╔══════════════════════════════════════════════════════════════╗
// ║  第三部分：API 桥接层 — 佘时裕                                  ║
// ║  单例模式，前端唯一入口                                          ║
// ╚══════════════════════════════════════════════════════════════╝

// 前置声明
class AuthService;
class BookService;
class BorrowService;
class ReservationService;
class OverdueScanner;
class LogService;

class ApiBridge {
public:
    // —— 单例 ——
    static ApiBridge& getInstance();
    // 输入: 数据库文件路径; 输出: 初始化是否成功
    bool initialize(const string& dbPath = "library.db");

    // ======== 认证 ========
    // 输入: 用户名, 密码(明文)
    // 输出: 用户对象; status.msg 含错误信息
    User login(const string& username, const string& password, Status& status);

    // 输入: 用户名, 密码, 姓名, 角色
    // 输出: 用户对象; status 含注册结果
    User registerUser(const string& username, const string& password,
                      const string& realName, const string& role, Status& status);

    // 输出: 当前登录用户
    User getCurrentUser() const;

    // ======== 检索 ========
    // done: false=执行中, true=已结束（供前端轮询）
    pair<vector<Book>, int> searchBooks(const string& query, int categoryId,
                                        int page, int pageSize,
                                        Status& status, bool& done);

    Book getBookDetail(int bookId);

    // done: 同上
    vector<Book> getRecommendations(int userId, Status& status, bool& done);

    // ======== 借阅 ========
    // done: false=执行中(含乐观锁重试), true=已结束
    BorrowRecord borrowBook(int userId, int bookId, Status& status, bool& done);
    BorrowRecord returnBook(int borrowId, Status& status, bool& done);
    BorrowRecord renewBook(int borrowId, Status& status, bool& done);

    // 输入: userId; 输出: 借阅记录列表
    vector<BorrowRecord> getMyBorrowRecords(int userId);

    // ======== 预约 ========
    // 输入: userId, bookId; status 返回结果
    // 输出: 预约记录（失败时 id=0）
    Reservation reserveBook(int userId, int bookId, Status& status);
    Status cancelReservation(int reservationId);
    vector<Reservation> getMyReservations(int userId);

    // ======== 管理 ========
    Status addBook(const Book& book);
    Status updateBook(const Book& book);
    Status deleteBook(int bookId, int adminId);
    vector<User> getAllUsers();
    Status disableUser(int userId, int adminId);
    Status enableUser(int userId);

    // ======== 统计 ========
    // done: false=执行中(汇总多表), true=已结束
    struct Stats { int totalBooks; int totalUsers; int activeBorrows;
                   int overdueCount; double totalFines; };
    Stats getStatistics(Status& status, bool& done);

private:
    ApiBridge();                                    // 禁止外部构造
    ~ApiBridge();
    ApiBridge(const ApiBridge&) = delete;           // 禁止拷贝
    ApiBridge& operator=(const ApiBridge&) = delete;

    // —— 各 Service 实例 ——
    unique_ptr<AuthService>        authSvc_;
    unique_ptr<BookService>        bookSvc_;
    unique_ptr<BorrowService>      borrowSvc_;
    unique_ptr<ReservationService> resSvc_;
    unique_ptr<OverdueScanner>     overdueScanner_;
    unique_ptr<LogService>         logSvc_;

    User currentUser_;             // 当前登录用户
    bool initialized_ = false;
};


// ╔══════════════════════════════════════════════════════════════╗
// ║  第四部分：业务服务层 — 佘时裕                                    ║
// ║  事务控制 + 并发安全 + 定时任务                                    ║
// ╚══════════════════════════════════════════════════════════════╝

// ----------------------------------------------------------------
//  S1. AuthService — 认证服务
// ----------------------------------------------------------------
class AuthService {
private:
    class UserDao* userDao_;

public:
    explicit AuthService(class UserDao* dao);

    // 输入: 用户名, 明文密码
    // 输出: 用户对象（失败时 id=0）; status.code 含错误码, status.msg 含中文信息
    User login(const string& username, const string& password, Status& status);

    // 输入: 用户名, 明文密码, 姓名, 角色
    // 输出: 用户对象（失败时 id=0）; status 含注册结果
    User registerUser(const string& username, const string& password,
                      const string& realName, const string& role, Status& status);
};


// ----------------------------------------------------------------
//  S2. BookService — 图书服务
// ----------------------------------------------------------------
class BookService {
private:
    class BookDao*     bookDao_;
    class LogService*  logSvc_;
    SearchIndex*       searchIdx_;
    RecommendEngine*   recommendEng_;
    CategoryTree*      categoryTree_;

public:
    BookService(class BookDao* bDao, class LogService* lSvc,
                SearchIndex* idx, RecommendEngine* rec, CategoryTree* cat);

    // done: false=执行中, true=已结束
    pair<vector<Book>, int> search(const string& query, int categoryId,
                                   int page, int pageSize,
                                   Status& status, bool& done);

    Book getDetail(int bookId);

    vector<Book> getRecommendations(int userId, int topN, Status& status, bool& done);

    // 管理操作（需要 adminId 记录日志）; status 返回结果
    Status addBook(const Book& book, int adminId);
    Status updateBook(const Book& book, int adminId);
    Status deleteBook(int bookId, int adminId);
};


// ----------------------------------------------------------------
//  S3. BorrowService（核心）— 借阅服务 + 乐观锁事务
// ----------------------------------------------------------------
class BorrowService {
private:
    class BorrowRecordDao* borrowDao_;
    class BookDao*         bookDao_;
    CreditEngine*          creditEng_;
    ReservationQueue*      resQueue_;

    static constexpr int MAX_RENEW       = 2;       // 最大续借次数
    static constexpr int RENEW_DAYS      = 15;      // 续借延长天数
    static constexpr int BORROW_DAYS     = 30;      // 借阅天数
    static constexpr int MAX_RETRY       = 3;       // 乐观锁最大重试次数

public:
    BorrowService(class BorrowRecordDao* brDao, class BookDao* bDao,
                  CreditEngine* credit, ReservationQueue* queue);

    // done: false=执行中(含乐观锁重试), true=已结束
    BorrowRecord borrowBook(int userId, int bookId, Status& status, bool& done);
    BorrowRecord returnBook(int borrowId, Status& status, bool& done);
    BorrowRecord renewBook(int borrowId, Status& status, bool& done);

    // 输入: userId; 输出: 借阅记录列表
    vector<BorrowRecord> getMyRecords(int userId);
};


// ----------------------------------------------------------------
//  S4. ReservationService — 预约服务
// ----------------------------------------------------------------
class ReservationService {
private:
    class ReservationDao* reserveDao_;
    ReservationQueue*     queue_;

public:
    ReservationService(class ReservationDao* dao, ReservationQueue* q);

    // 输入: userId, bookId; status 返回结果
    // 输出: 预约记录（失败时 id=0, status.msg 含原因）
    Reservation reserveBook(int userId, int bookId, Status& status);

    // 输入: reservationId
    // 输出: Status 对象，包含取消结果
    Status cancelReservation(int reservationId);

    // 输入: userId; 输出: 预约列表
    vector<Reservation> getMyReservations(int userId);

    // 归还触发: 通知队首用户（内部调用 queue_.dequeue()）
    void processReturn(int bookId);

    // 定时扫描: 处理超时预约（内部调用 queue_.processExpired()）
    void processExpired();
};


// ----------------------------------------------------------------
//  S5. OverdueScanner — 逾期扫描定时任务
// ----------------------------------------------------------------
class OverdueScanner {
private:
    class BorrowRecordDao* borrowDao_;
    class BookDao*         bookDao_;
    CreditEngine*          creditEng_;

public:
    OverdueScanner(class BorrowRecordDao* brDao, class BookDao* bDao,
                   CreditEngine* credit);

    // 执行一次扫描: 找到所有逾期记录 → 更新状态 → 计算罚款 → 扣信用分
    void scan();
};


// ----------------------------------------------------------------
//  S6. LogService — 审计日志服务
// ----------------------------------------------------------------
class LogService {
private:
    class LogDao* logDao_;

public:
    explicit LogService(class LogDao* dao);

    // 输入: 管理员ID, 操作类型, 操作对象, 详情
    void log(int adminId, const string& action,
             const string& target, const string& detail);

    // 输入: 管理员ID; 输出: 该管理员的全部操作日志
    vector<LogEntry> getAdminLogs(int adminId);

    // 输入: 天数; 输出: 最近N天的全部操作日志
    vector<LogEntry> getRecentLogs(int days);
};


// ╔══════════════════════════════════════════════════════════════╗
// ║  第五部分：数据持久层 (DAO) — 佘时裕                              ║
// ║  SQLite + 6张表 + 索引 + 触发器                                  ║
// ╚══════════════════════════════════════════════════════════════╝

// ----------------------------------------------------------------
//  D1. DatabaseManager — 数据库管理器
// ----------------------------------------------------------------
class DatabaseManager {
private:
    void* db_;           // sqlite3* 指针

    void createTables();
    void createIndexes();
    void createTriggers();

public:
    DatabaseManager();
    ~DatabaseManager();

    // 输入: 数据库文件路径
    // 输出: Status, code=OK表示成功，msg含具体错误
    Status open(const string& dbPath = "library.db");
    void close();

    // 事务控制
    Status beginTransaction();
    Status commit();
    Status rollback();

    // 输入: SQL 语句; 输出: 执行状态
    Status execute(const string& sql);

    // 输出: 底层 sqlite3* 句柄（供 DAO 使用）
    void* getHandle();
};


// ----------------------------------------------------------------
//  D2~D7: 各 DAO 类（封装 SQL 操作，对外暴露业务接口）
// ----------------------------------------------------------------

class UserDao {
private:
    DatabaseManager& db_;
    User rowToUser(void* stmt) const;

public:
    explicit UserDao(DatabaseManager& db);

    User findByUsername(const string& username);
    User findById(int userId);
    vector<User> findAll();
    User insert(const User& user, Status& status);       // 返回带id的User
    Status update(const User& user);
    Status updateCredit(int userId, int credit);
    Status updateStatus(int userId, const string& status);
};


class BookDao {
private:
    DatabaseManager& db_;
    Book rowToBook(void* stmt) const;

public:
    explicit BookDao(DatabaseManager& db);

    Book findById(int bookId);
    vector<Book> findByIds(const vector<int>& ids);
    vector<Book> findByCategory(int categoryId);
    vector<Book> findAll();
    Book insert(const Book& book, Status& status);       // 返回带id的Book
    Status update(const Book& book);
    Status deleteBook(int bookId);

    // 乐观锁更新库存: 输入 delta（+1/-1）, 期望版本号
    // status.code: OK / ERR_VERSION_CONFLICT
    Status updateStockWithLock(int bookId, int delta, int expectedVersion);
};


class BorrowRecordDao {
private:
    DatabaseManager& db_;
    BorrowRecord rowToRecord(void* stmt) const;

public:
    explicit BorrowRecordDao(DatabaseManager& db);

    BorrowRecord insert(const BorrowRecord& r, Status& status);
    vector<BorrowRecord> findByUser(int userId);
    vector<BorrowRecord> findOverdue();
    Status updateReturn(int borrowId, const string& returnDate);
    Status updateRenew(int borrowId, const string& newDueDate);
    Status updateStatus(int borrowId, const string& status);
};


class ReservationDao {
private:
    DatabaseManager& db_;
    Reservation rowToReservation(void* stmt) const;

public:
    explicit ReservationDao(DatabaseManager& db);

    Reservation insert(const Reservation& r, Status& status);
    vector<Reservation> findByUser(int userId);
    vector<Reservation> findByBook(int bookId);
    Status updateStatus(int reservationId, const string& status);
    vector<Reservation> findExpired(int timeoutHours);
};


class CategoryDao {
private:
    DatabaseManager& db_;

public:
    explicit CategoryDao(DatabaseManager& db);

    vector<Category> findAll();
    Category insert(const Category& c, Status& status);
    Status update(const Category& c);
    Status deleteCategory(int categoryId);
};


class LogDao {
private:
    DatabaseManager& db_;

public:
    explicit LogDao(DatabaseManager& db);

    LogEntry insert(const LogEntry& log, Status& status);
    vector<LogEntry> findByAdmin(int adminId);
    vector<LogEntry> findRecent(int days);
};

#endif // BACKEND_H
