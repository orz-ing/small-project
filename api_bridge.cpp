// api_bridge.cpp — API 桥接层（单例，前端唯一入口）
#include "backend.h"
using namespace std;

// ===== 单例 =====
ApiBridge& ApiBridge::getInstance()
{
    // TODO: static ApiBridge instance;
}

// ===== 初始化 =====
// 输入: 数据库文件路径
// 输出: true=初始化成功（打开数据库 + 创建各 Service）
bool ApiBridge::initialize(const string& dbPath)
{
    // TODO: 初始化 DatabaseManager → DAO → Algorithm → Service
}

// ===== 登录 =====
// 输入: username(账号), password(明文密码)
// 输出: User 对象; id=0 表示登录失败
User ApiBridge::login(const string& username, const string& password)
{
    // TODO: authSvc_->login(username, password)
}

// ===== 注册 =====
// 输入: username, password, realName, role("reader"/"admin")
// 输出: true=注册成功; false=用户名已存在
bool ApiBridge::registerUser(const string& username, const string& password,
    const string& realName, const string& role)
{
    // TODO: authSvc_->registerUser(...)
}

// ===== 获取当前用户 =====
User ApiBridge::getCurrentUser() const
{
    // TODO: return currentUser_
}

// ===== 搜索图书 =====
// 输入: query(关键词), categoryId(-1=全部), page(页码), pageSize(每页条数)
// 输出: (图书列表, 总数量)
pair<vector<Book>, int> ApiBridge::searchBooks(const string& query, int categoryId,
    int page, int pageSize)
{
    // TODO: bookSvc_->search(...)
}

// ===== 图书详情 =====
// 输入: bookId
// 输出: Book 对象; id=0 表示不存在
Book ApiBridge::getBookDetail(int bookId)
{
    // TODO: bookSvc_->getDetail(bookId)
}

// ===== 推荐 =====
// 输入: userId
// 输出: 推荐图书列表
vector<Book> ApiBridge::getRecommendations(int userId)
{
    // TODO: bookSvc_->getRecommendations(userId, 10)
}

// ===== 借书 =====
// 输入: userId, bookId
// 输出: ""=成功; 否则返回错误原因字符串
string ApiBridge::borrowBook(int userId, int bookId)
{
    // TODO: borrowSvc_->borrowBook(userId, bookId)
}

// ===== 还书 =====
// 输入: borrowId
// 输出: ""=成功; 否则返回错误原因
string ApiBridge::returnBook(int borrowId)
{
    // TODO: borrowSvc_->returnBook(borrowId)
}

// ===== 续借 =====
// 输入: borrowId
// 输出: ""=成功; 否则返回错误原因
string ApiBridge::renewBook(int borrowId)
{
    // TODO: borrowSvc_->renewBook(borrowId)
}

// ===== 我的借阅记录 =====
// 输入: userId
// 输出: 借阅记录列表
vector<BorrowRecord> ApiBridge::getMyBorrowRecords(int userId)
{
    // TODO: borrowSvc_->getMyRecords(userId)
}

// ===== 预约 =====
// 输入: userId, bookId
// 输出: 排队位置（从1开始）; -1=预约失败
int ApiBridge::reserveBook(int userId, int bookId)
{
    // TODO: resSvc_->reserveBook(userId, bookId)
}

// ===== 取消预约 =====
// 输入: reservationId
// 输出: true=取消成功
bool ApiBridge::cancelReservation(int reservationId)
{
    // TODO: resSvc_->cancelReservation(reservationId)
}

// ===== 我的预约列表 =====
// 输入: userId
// 输出: 预约记录列表
vector<Reservation> ApiBridge::getMyReservations(int userId)
{
    // TODO: resSvc_->getMyReservations(userId)
}

// ===== 添加图书（管理员）=====
// 输入: Book 对象（id 字段忽略，自动生成）
// 输出: true=添加成功
bool ApiBridge::addBook(const Book& book)
{
    // TODO: bookSvc_->addBook(book, currentUser_.getId())
}

// ===== 更新图书（管理员）=====
// 输入: Book 对象（按 id 更新）
// 输出: true=更新成功
bool ApiBridge::updateBook(const Book& book)
{
    // TODO: bookSvc_->updateBook(book, currentUser_.getId())
}

// ===== 删除图书（管理员）=====
// 输入: bookId, adminId
// 输出: true=删除成功
bool ApiBridge::deleteBook(int bookId, int adminId)
{
    // TODO: bookSvc_->deleteBook(bookId, adminId)
}

// ===== 获取全部用户（管理员）=====
// 输出: 所有用户列表
vector<User> ApiBridge::getAllUsers()
{
    // TODO
}

// ===== 禁用用户（管理员）=====
// 输入: userId, adminId
// 输出: true=操作成功
bool ApiBridge::disableUser(int userId, int adminId)
{
    // TODO
}

// ===== 启用用户（管理员）=====
// 输入: userId
// 输出: true=操作成功
bool ApiBridge::enableUser(int userId)
{
    // TODO
}

// ===== 获取统计数据 =====
// 输出: 总图书数/总用户数/活跃借阅数/逾期数/总罚款
ApiBridge::Stats ApiBridge::getStatistics()
{
    // TODO: 汇总各 DAO 数据
}
