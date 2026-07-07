/******************************************************************************
 * backend.h 鈥?鍥句功绠＄悊绯荤粺鍚庣缁熶竴澶存枃浠讹紙閲嶆瀯鐗?v2.0锛?
 *
 * 鏋舵瀯灞傛锛堣嚜涓婅€屼笅锛夛細
 *   ApiBridge锛堝崟渚嬶紝鍓嶇鍞竴鍏ュ彛锛?
 *       鈫?
 *   Service 灞傦紙浜嬪姟鎺у埗 + 骞跺彂 + 瀹氭椂浠诲姟锛?
 *       鈫?
 *   DAO 灞傦紙鏁版嵁搴?CRUD + SQLite锛?
 *       鈫?
 *   Algorithm 灞傦紙绾嚱鏁帮紝鏃犵姸鎬侊紝鐙珛鍙祴锛?
 ******************************************************************************/

#ifndef BACKEND_H
#define BACKEND_H

// ========== C++ 鏍囧噯搴?==========
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


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 鍏ㄥ眬鐘舵€佺爜浣撶郴锛堟墍鏈夊彲澶辫触鎿嶄綔鐨勭粺涓€杩斿洖鍊硷級                      鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

enum class StatusCode {
    OK = 0,
    ERR_DB_OPEN,               // 鏁版嵁搴撴墦寮€澶辫触
    ERR_DB_EXEC,               // SQL 鎵ц澶辫触
    ERR_DB_TRANSACTION,        // 浜嬪姟澶辫触
    ERR_USER_NOT_FOUND,        // 鐢ㄦ埛涓嶅瓨鍦?
    ERR_USER_DISABLED,         // 鐢ㄦ埛琚鐢?
    ERR_USER_EXISTS,           // 鐢ㄦ埛鍚嶅凡瀛樺湪
    ERR_WRONG_PASSWORD,        // 瀵嗙爜閿欒
    ERR_BOOK_NOT_FOUND,        // 鍥句功涓嶅瓨鍦?
    ERR_BOOK_OUT_OF_STOCK,     // 搴撳瓨涓嶈冻
    ERR_BOOK_HAS_BORROW,       // 鏈夋湭杩樺€熼槄
    ERR_BORROW_NOT_FOUND,      // 鍊熼槄璁板綍涓嶅瓨鍦?
    ERR_RENEW_LIMIT,           // 缁€熷凡杈句笂闄?
    ERR_RENEW_OVERDUE,         // 宸查€炬湡涓嶈兘缁€?
    ERR_CREDIT_TOO_LOW,        // 淇＄敤鍒嗕笉瓒?
    ERR_VERSION_CONFLICT,      // 涔愯閿佸啿绐?
    ERR_RESERVATION_EXISTS,    // 宸叉湁棰勭害
    ERR_RESERVATION_NOT_FOUND, // 棰勭害涓嶅瓨鍦?
    ERR_RESERVATION_EXPIRED,   // 棰勭害宸茶繃鏈?
    ERR_CATEGORY_HAS_CHILD,    // 鏈夊瓙鍒嗙被
    ERR_CATEGORY_NOT_FOUND,    // 鍒嗙被涓嶅瓨鍦?
    ERR_PERMISSION_DENIED,     // 鏉冮檺涓嶈冻
    ERR_FILE_OPEN,             // 鏂囦欢鎵撳紑澶辫触
    ERR_FILE_WRITE,            // 鏂囦欢鍐欏叆澶辫触
    ERR_UNKNOWN                // 鏈煡閿欒
};

// Status 鐘舵€佸寘 鈥?璋冪敤鏂瑰彲 if (!status) 鍒ゆ柇骞惰緭鍑?status.msg 缁欑敤鎴?
struct Status {
    StatusCode code = StatusCode::OK;
    string msg;
    Status() = default;
    Status(StatusCode c, string m = "") : code(c), msg(m) {}
    bool ok()            const { return code == StatusCode::OK; }
    operator bool()      const { return code == StatusCode::OK; }
    string getCodeName() const;   // 杩斿洖鐘舵€佺爜鑻辨枃鍚嶏紙鐢ㄤ簬鏃ュ織锛?
};
inline Status StatusOK()                                  { return {StatusCode::OK, ""}; }
inline Status StatusErr(StatusCode c, const string& m)    { return {c, m}; }

// ================================================================
// 銆愯繍琛岀姸鎬佹爣璇嗐€?
// 瀵逛簬鍙兘鑰楁椂杈冮暱鐨勬搷浣滐紙鎼滅储銆佸€熼槄浜嬪姟銆佹壒閲忓鍏ョ瓑锛夛紝
// 璋冪敤鏂逛紶鍏ヤ竴涓?bool& done 寮曠敤鍙橀噺锛岀敱琚皟鐢ㄥ嚱鏁扮淮鎶わ細
//
//   done = false  鈫? 鎿嶄綔姝ｅ湪鎵ц涓紙璋冪敤鏂瑰彲杞姝ゅ彉閲忥級
//   done = true   鈫? 鎿嶄綔宸茬粨鏉燂紙鎴愬姛鎴栧け璐ワ紝閰嶅悎 Status 鍒ゆ柇缁撴灉锛?
//
// 绀轰緥锛?
//   bool done = false;
//   auto result = apiBridge.searchBooks("C++", -1, 1, 20, status, done);
//   while (!done) { QApplication::processEvents(); }  // 鍓嶇淇濇寔鍝嶅簲
//   if (status.ok()) { /* 浣跨敤 result */ }
// ================================================================


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 绗竴閮ㄥ垎锛氭暟鎹ā鍨嬶紙Model锛?                                 鈺?
// 鈺? 鎵€鏈夋暟鎹瓧娈典负 private锛岄€氳繃 public 鏂规硶璁块棶                   鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

// ----------------------------------------------------------------
//  M1. User 鈥?鐢ㄦ埛鍩虹被
//      娲剧敓锛歊eader锛堣鑰咃級/ Admin锛堢鐞嗗憳锛?
// ----------------------------------------------------------------
class User {
private:
    int    id_;
    string username_;          // 鍞竴锛岀櫥褰曡处鍙?
    string passwordHash_;      // SHA-256 鍝堝笇锛岀姝㈡槑鏂?
    string realName_;          // 鐪熷疄濮撳悕
    string role_;              // "reader" | "admin"
    int    credit_;            // 淇＄敤鍒嗭紝鑼冨洿 [0, 100]锛屽垵濮?100
    string status_;            // "active" | "disabled"
    string phone_;
    string email_;
    string createTime_;        // "YYYY-MM-DD HH:MM:SS"

public:
    // 鈥斺€?鏋勯€?鈥斺€?
    User();
    User(const string& username, const string& passwordHash,
         const string& realName, const string& role);

    // 鈥斺€?Getter锛堝彧璇昏闂級 鈥斺€?
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

    // 鈥斺€?Setter锛堝彈鎺т慨鏀癸級 鈥斺€?
    void setId(int id)                  { id_ = id; }
    void setPasswordHash(const string& h) { passwordHash_ = h; }
    void setUsername(const string& u)       { username_ = u; }
    void setRealName(const string& n)     { realName_ = n; }
    void setPhone(const string& p)        { phone_ = p; }
    void setEmail(const string& e)        { email_ = e; }
    void setCreateTime(const string& t)   { createTime_ = t; }

    // 鈥斺€?淇＄敤鍒嗘搷浣?鈥斺€?
    // 杈撳叆: delta锛堟鏁板姞/璐熸暟鎵ｏ級锛涜緭鍑? 鎿嶄綔鍚庣殑淇＄敤鍒嗭紙鑷姩 clamp 鍒?[0,100]锛?
    int  adjustCredit(int delta);
    void setCredit(int c)                { credit_ = max(0, min(100, c)); }

    // 鈥斺€?鐘舵€佹搷浣?鈥斺€?
    // 杈撳叆: status("active"/"disabled")锛涜緭鍑? 鏄惁璁剧疆鎴愬姛
    bool setStatus(const string& s);
    bool isActive() const { return status_ == "active"; }
    bool isAdmin()  const { return role_ == "admin"; }

    // 鈥斺€?鏉冮檺鍒ゆ柇 鈥斺€?
    // 杈撳嚭: 淇＄敤鍒?鈮?60 涓旇处鍙锋湭绂佺敤 鈫?true
    bool canBorrow() const;
};


// ----------------------------------------------------------------
//  M2. Admin 鈥?绠＄悊鍛橈紙缁ф壙 User锛?
//      鎷ユ湁棰濆鐨勭鐞嗘潈闄?
// ----------------------------------------------------------------
class Admin : public User {
private:
    int    adminLevel_;        // 鏉冮檺绾у埆: 1=鏅€氱鐞嗗憳, 2=瓒呯骇绠＄悊鍛?
    string department_;        // 鎵€灞為儴闂?

public:
    Admin();
    Admin(const string& username, const string& passwordHash,
          const string& realName, int level = 1);

    // 鈥斺€?Getter 鈥斺€?
    int    getAdminLevel() const { return adminLevel_; }
    string getDepartment()  const { return department_; }

    // 鈥斺€?Setter 鈥斺€?
    void setAdminLevel(int l)   { adminLevel_ = l; }
    void setDepartment(const string& d) { department_ = d; }

    // 鈥斺€?鏉冮檺妫€鏌?鈥斺€?
    // 杈撳嚭: level>=2 鍒欐嫢鏈夊垹闄ゅ浘涔?绂佺敤鐢ㄦ埛绛夋晱鎰熸搷浣滄潈闄?
    bool canDeleteBook()   const { return adminLevel_ >= 2; }
    bool canDisableUser()  const { return adminLevel_ >= 2; }
    bool canViewAllLogs()  const { return adminLevel_ >= 2; }
};


// ----------------------------------------------------------------
//  M3. Book 鈥?鍥句功绫?
//      灏佽鍥句功搴撳瓨鐨勫鍑忛€昏緫锛岀‘淇濇暟鎹竴鑷存€?
// ----------------------------------------------------------------
class Book {
private:
    int    id_;
    string isbn_;
    string title_;
    string author_;
    string publisher_;
    string publishDate_;
    int    categoryId_;          // 鍒嗙被 ID锛屽叧鑱?Category
    int    stock_;               // 褰撳墠鍙€熷簱瀛橈紙鈮?0锛?
    int    totalStock_;          // 鎬诲鏈暟锛堚墺 stock锛?
    int    version_;             // 涔愯閿佺増鏈彿
    string location_;            // 涔︽灦浣嶇疆
    string description_;         // 绠€浠?
    string coverUrl_;            // 灏侀潰鍥剧墖 URL

public:
    Book();

    // 鈥斺€?Getter 鈥斺€?
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

    // 鈥斺€?Setter 鈥斺€?
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

    // 鈥斺€?搴撳瓨鎿嶄綔锛堟牳蹇冿級 鈥斺€?
    // 杈撳叆: delta锛?1褰掕繕, -1鍊熷嚭锛夛紱杈撳嚭: 鎿嶄綔鍚庣殑搴撳瓨閲忥紙澶辫触杩斿洖 -1锛?
    int  adjustStock(int delta);

    // 杈撳嚭: stock > 0 鍒欏彲鍊?
    bool isAvailable()   const;

    // 涔愯閿佺増鏈彿閫掑
    void bumpVersion()   { version_++; }
};


// ----------------------------------------------------------------
//  M4. BorrowRecord 鈥?鍊熼槄璁板綍
// ----------------------------------------------------------------
class BorrowRecord {
private:
    int    id_;
    int    userId_;
    int    bookId_;
    string borrowDate_;       // "YYYY-MM-DD"
    string dueDate_;          // 搴旇繕鏃ユ湡 = borrowDate + 30澶?
    string returnDate_;       // 绌轰覆 = 鏈綊杩?
    string status_;           // "borrowing" | "returned" | "overdue"
    int    renewCount_;       // 缁€熸鏁帮紝涓婇檺 2

public:
    BorrowRecord();

    // 鈥斺€?Getter 鈥斺€?
    int    getId()         const { return id_; }
    int    getUserId()     const { return userId_; }
    int    getBookId()     const { return bookId_; }
    string getBorrowDate() const { return borrowDate_; }
    string getDueDate()    const { return dueDate_; }
    string getReturnDate() const { return returnDate_; }
    string getStatus()     const { return status_; }
    int    getRenewCount() const { return renewCount_; }

    // 鈥斺€?Setter 鈥斺€?
    void setId(int i)                { id_ = i; }
    void setUserId(int u)            { userId_ = u; }
    void setBookId(int b)            { bookId_ = b; }
    void setBorrowDate(const string& d) { borrowDate_ = d; }
    void setDueDate(const string& d)    { dueDate_ = d; }
    void setReturnDate(const string& d) { returnDate_ = d; }
    void setRenewCount(int r)           { renewCount_ = r; }

    // 鈥斺€?鐘舵€佹搷浣?鈥斺€?
    // 杈撳叆: status("borrowing"/"returned"/"overdue"); 杈撳嚭: 鏄惁鍚堟硶
    bool setStatus(const string& s);

    // 杈撳叆: 褰撳墠鏃ユ湡("YYYY-MM-DD"); 杈撳嚭: 鏄惁宸查€炬湡
    bool isOverdue(const string& today) const;

    // 杈撳叆: 缁€熷欢闀垮ぉ鏁?榛樿15)锛涜緭鍑? 鏂扮殑搴旇繕鏃ユ湡锛堝凡杈句笂闄愯繑鍥炵┖涓诧級
    string renew(int extraDays = 15);
};


// ----------------------------------------------------------------
//  M5. Reservation 鈥?棰勭害璁板綍
// ----------------------------------------------------------------
class Reservation {
private:
    int    id_;
    int    userId_;
    int    bookId_;
    string reserveDate_;      // 棰勭害鏃堕棿
    string expireDate_;       // 杩囨湡鏃堕棿锛?48h锛?
    string status_;           // "pending" | "fulfilled" | "cancelled" | "expired"
    int    priority_;         // 0=鏅€? 1=浼樺厛(淇＄敤鈮?0)

public:
    Reservation();

    // 鈥斺€?Getter 鈥斺€?
    int    getId()          const { return id_; }
    int    getUserId()      const { return userId_; }
    int    getBookId()      const { return bookId_; }
    string getReserveDate() const { return reserveDate_; }
    string getExpireDate()  const { return expireDate_; }
    string getStatus()      const { return status_; }
    int    getPriority()    const { return priority_; }

    // 鈥斺€?Setter 鈥斺€?
    void setId(int i)               { id_ = i; }
    void setUserId(int u)           { userId_ = u; }
    void setBookId(int b)           { bookId_ = b; }
    void setReserveDate(const string& d) { reserveDate_ = d; }
    void setExpireDate(const string& d)  { expireDate_ = d; }
    void setPriority(int p)             { priority_ = p; }

    // 鈥斺€?鐘舵€佹搷浣?鈥斺€?
    bool setStatus(const string& s);

    // 杈撳叆: 褰撳墠鏃堕棿; 杈撳嚭: 鏄惁宸茶繃鏈?
    bool isExpired(const string& now) const;
};


// ----------------------------------------------------------------
//  M6. Category 鈥?鍥句功鍒嗙被
// ----------------------------------------------------------------
class Category {
private:
    int    id_;
    string name_;
    int    parentId_;         // 鐖跺垎绫?ID锛? 琛ㄧず鏍瑰垎绫?
    int    level_;            // 灞傜骇娣卞害锛堟牴涓?0锛?

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

    // 杈撳嚭: parentId==0 鍒欎负鏍瑰垎绫?
    bool isRoot() const { return parentId_ == 0; }
};


// ----------------------------------------------------------------
//  M7. LogEntry 鈥?鏃ュ織璁板綍
// ----------------------------------------------------------------
class LogEntry {
private:
    int    id_;
    int    adminId_;
    string action_;          // 鎿嶄綔绫诲瀷锛屽 "delete_book"
    string target_;          // 鎿嶄綔瀵硅薄鎻忚堪
    string detail_;          // 璇︽儏
    string time_;            // 鏃堕棿鎴?

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


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 绗簩閮ㄥ垎锛氱畻娉曞紩鎿庯紙Algorithm锛?鈥?鐜嬪崥娑?                      鈺?
// 鈺? 绾嚱鏁帮紝鏃犵姸鎬侊紝鏃?DB 渚濊禆锛屽彲鐙珛鍗曞厓娴嬭瘯                       鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

// ----------------------------------------------------------------
//  A1. CreditEngine 鈥?淇＄敤绉垎寮曟搸
// ----------------------------------------------------------------
class CreditEngine {
public:
    static constexpr int INITIAL_SCORE    = 100;
    static constexpr int BORROW_THRESHOLD = 60;    // 浣庝簬姝ゅ垎绂佹鍊熼槄
    static constexpr int MAX_SCORE        = 100;
    static constexpr int PENALTY_OVERDUE  = -10;   // 閫炬湡褰掕繕鎵ｅ垎
    static constexpr int PENALTY_DAMAGE   = -20;   // 鎹熷潖鎵ｅ垎
    static constexpr int PENALTY_SEVERE   = -30;   // 涓ラ噸閫炬湡(>30澶?
    static constexpr int REWARD_ON_TIME   = +5;    // 鎸夋椂褰掕繕鍔犲垎
    static constexpr int REWARD_DONATE    = +10;   // 鎹愯禒鍔犲垎

    // 杈撳叆: 褰撳墠淇＄敤鍒? 鎿嶄綔绫诲瀷("overdue"/"on_time"/"damage"/"donate")
    // 杈撳嚭: 鏂扮殑淇＄敤鍒嗭紙鑷姩 clamp 鍒?[0, MAX_SCORE]锛?
    static int updateScore(int currentScore, const string& action);

    // 杈撳叆: 淇＄敤鍒? 杈撳嚭: 鏄惁鍙€熼槄
    static bool canBorrow(int score);

    // 杈撳叆: 淇＄敤鍒? 杈撳嚭: "浼樼"(鈮?0) | "鑹ソ"(鈮?5) | "涓€鑸?(鈮?0) | "杈冨樊"(<60)
    static string getLevel(int score);
};


// ----------------------------------------------------------------
//  A2. FineCalculator 鈥?闃舵璐圭巼缃氭璁＄畻鍣?
// ----------------------------------------------------------------
class FineCalculator {
private:
    static constexpr double RATE_TIER1  = 0.5;    // 1-7澶?
    static constexpr double RATE_TIER2  = 1.0;    // 8-30澶?
    static constexpr double RATE_TIER3  = 2.0;    // >30澶?
    static constexpr int    TIER1_DAYS  = 7;
    static constexpr int    TIER2_DAYS  = 30;
    static constexpr double MAX_FINE    = 100.0;

public:
    // 杈撳叆: dueDate(搴旇繕鏃ユ湡"YYYY-MM-DD"), returnDate(瀹為檯褰掕繕鏃ユ湡)
    // 杈撳嚭: 缃氭閲戦锛堝厓锛夛紝0 琛ㄧず鏈€炬湡
    static double calcFine(const string& dueDate, const string& returnDate);
};


// ----------------------------------------------------------------
//  A3. SearchIndex 鈥?鍊掓帓绱㈠紩鎼滅储寮曟搸
// ----------------------------------------------------------------
struct SearchResult {
    int    bookId;
    double score;        // 鐩稿叧鎬у緱鍒嗭紙瓒婇珮瓒婄浉鍏筹級
};

class SearchIndex {
private:
    // 鏍稿績: 鍏抽敭璇?鈫?bookId 闆嗗悎
    unordered_map<string, set<int>> invertedIndex_;
    // 杈呭姪: bookId 鈫?鍊熼槄娆℃暟锛堢儹搴︽帓搴忕敤锛?
    unordered_map<int, int> borrowCount_;

    // 鍒嗚瘝: 杈撳叆鏂囨湰 鈫?杈撳嚭鍏抽敭璇嶅垪琛?
    vector<string> tokenize(const string& text) const;

public:
    // 杈撳叆: 鍏ㄩ儴鍥句功鍒楄〃; 杈撳嚭: 鏃狅紙鏋勫缓绱㈠紩锛?
    void buildIndex(const vector<Book>& books);

    // 杈撳叆: 鎼滅储鍏抽敭璇? 杩斿洖鏁伴噺; 杈撳嚭: 鎸夌浉鍏虫€ч檷搴忕殑鎼滅储缁撴灉
    vector<SearchResult> search(const string& query, int topN = 20) const;

    // 澧為噺鏇存柊
    void addBook(const Book& book);
    void removeBook(int bookId);
};


// ----------------------------------------------------------------
//  A4. RecommendEngine 鈥?鍗忓悓杩囨护鎺ㄨ崘寮曟搸
// ----------------------------------------------------------------
class RecommendEngine {
private:
    // userId 鈫?鍊熻繃鐨?bookId 闆嗗悎
    unordered_map<int, set<int>> userBooks_;
    // (bookA, bookB) 鈫?鍚屾椂琚€熺殑娆℃暟
    map<pair<int,int>, int> cooccurrence_;

    // 杈撳叆: 涓や釜闆嗗悎; 杈撳嚭: Jaccard 鐩镐技搴?[0,1]
    double jaccardSimilarity(const set<int>& a, const set<int>& b) const;

public:
    // 杈撳叆: 鎵€鏈夊€熼槄璁板綍; 杈撳嚭: 鏃狅紙鏋勫缓鍏辩幇鐭╅樀锛?
    void buildMatrix(const vector<BorrowRecord>& records);

    // 杈撳叆: 鐩爣鐢ㄦ埛ID, 鎺ㄨ崘鏁伴噺, 宸插€熻褰? 鍏ㄩ儴鍥句功鍒楄〃
    // 杈撳嚭: 鎺ㄨ崘鍥句功ID鍒楄〃锛堟寜鎺ㄨ崘鍒嗘暟闄嶅簭锛?
    vector<int> recommend(int targetUserId, int topN,
                          const vector<BorrowRecord>& userHistory,
                          const vector<Book>& allBooks) const;

    // 瀹炴椂鏇存柊: 鏈夋柊鍊熼槄鏃惰皟鐢?
    void onBorrow(int userId, int bookId);
};


// ----------------------------------------------------------------
//  A5. CategoryTree 鈥?鍒嗙被鏍?
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
    // 杈撳叆: 鎵佸钩鍒嗙被鍒楄〃; 杈撳嚭: 鏃狅紙鏋勫缓鏍戯級
    void buildTree(const vector<Category>& categories);

    // 杈撳叆: 鍒嗙被ID; 杈撳嚭: 鐩存帴瀛愬垎绫籌D鍒楄〃
    vector<int> getChildren(int categoryId) const;

    // 杈撳叆: 鍒嗙被ID; 杈撳嚭: 鎵€鏈夊瓙瀛欏垎绫籌D锛堥€掑綊灞曞紑锛?
    vector<int> getAllDescendants(int categoryId) const;

    // 杈撳叆: 鍒嗙被ID; 杈撳嚭: 浠庢牴鍒拌鍒嗙被鐨勮矾寰勶紙濡?"璁＄畻鏈?缂栫▼璇█/C++"锛?
    string getPath(int categoryId) const;

    // 杈撳叆: 鍒嗙被鍚? 杈撳嚭: 鍒嗙被ID锛?1 鏈壘鍒帮級
    int findByName(const string& name) const;
};


// ----------------------------------------------------------------
//  A6. ReservationQueue 鈥?FIFO 棰勭害闃熷垪锛堢嚎绋嬪畨鍏級
// ----------------------------------------------------------------
class ReservationQueue {
private:
    struct QueueItem {
        int userId;
        int priority;      // 0=鏅€? 1=浼樺厛
        time_t timestamp;
    };
    // bookId 鈫?绛夊緟闃熷垪
    unordered_map<int, queue<QueueItem>> queues_;
    mutable mutex mtx_;

public:
    // 杈撳叆: bookId, userId, priority(0/1)
    // 杈撳嚭: 鎺掗槦浣嶇疆锛堜粠 1 寮€濮嬶級锛?1 琛ㄧず宸插湪闃熷垪涓?
    int  enqueue(int bookId, int userId, int priority = 0);

    // 杈撳叆: bookId; 杈撳嚭: 闃熼 userId锛?1 琛ㄧず闃熷垪绌猴級
    int  dequeue(int bookId);

    // 杈撳叆: bookId, userId
    void cancel(int bookId, int userId);

    // 杈撳叆: bookId, userId; 杈撳嚭: 鎺掗槦浣嶇疆锛?1 涓嶅湪闃熷垪锛?
    int  getPosition(int bookId, int userId) const;

    // 杈撳叆: 瓒呮椂灏忔椂鏁?榛樿48); 杈撳嚭: (bookId, userId) 瓒呮椂棰勭害鍒楄〃
    vector<pair<int,int>> processExpired(int timeoutHours = 48);

    // 杈撳叆: bookId; 杈撳嚭: 闃熷垪闀垮害
    int  size(int bookId) const;

    // 杈撳叆: bookId; 杈撳嚭: 鏌ョ湅闃熼锛堜笉寮瑰嚭锛?1 琛ㄧず绌猴級
    int  peekFront(int bookId) const;
};


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 绗笁閮ㄥ垎锛欰PI 妗ユ帴灞?鈥?浣樻椂瑁?                                 鈺?
// 鈺? 鍗曚緥妯″紡锛屽墠绔敮涓€鍏ュ彛                                          鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

// 鍓嶇疆澹版槑
class AuthService;
class BookService;
class BorrowService;
class ReservationService;
class OverdueScanner;
class LogService;

class ApiBridge {
public:
    // 鈥斺€?鍗曚緥 鈥斺€?
    static ApiBridge& getInstance();
    // 杈撳叆: 鏁版嵁搴撴枃浠惰矾寰? 杈撳嚭: 鍒濆鍖栨槸鍚︽垚鍔?
    bool initialize(const string& dbPath = "library.db");

    // ======== 璁よ瘉 ========
    // 杈撳叆: 鐢ㄦ埛鍚? 瀵嗙爜(鏄庢枃)
    // 杈撳嚭: 鐢ㄦ埛瀵硅薄; status.msg 鍚敊璇俊鎭?
    User login(const string& username, const string& password, Status& status);

    // 杈撳叆: 鐢ㄦ埛鍚? 瀵嗙爜, 濮撳悕, 瑙掕壊
    // 杈撳嚭: 鐢ㄦ埛瀵硅薄; status 鍚敞鍐岀粨鏋?
    User registerUser(const string& username, const string& password,
                      const string& realName, const string& role, Status& status);

    // 杈撳嚭: 褰撳墠鐧诲綍鐢ㄦ埛
    User getCurrentUser() const;

    // ======== 妫€绱?========
    // done: false=鎵ц涓? true=宸茬粨鏉燂紙渚涘墠绔疆璇級
    pair<vector<Book>, int> searchBooks(const string& query, int categoryId,
                                        int page, int pageSize,
                                        Status& status, bool& done);

    Book getBookDetail(int bookId);

    // done: 鍚屼笂
    vector<Book> getRecommendations(int userId, Status& status, bool& done);

    // ======== 鍊熼槄 ========
    // done: false=鎵ц涓?鍚箰瑙傞攣閲嶈瘯), true=宸茬粨鏉?
    BorrowRecord borrowBook(int userId, int bookId, Status& status, bool& done);
    BorrowRecord returnBook(int borrowId, Status& status, bool& done);
    BorrowRecord renewBook(int borrowId, Status& status, bool& done);

    // 杈撳叆: userId; 杈撳嚭: 鍊熼槄璁板綍鍒楄〃
    vector<BorrowRecord> getMyBorrowRecords(int userId);

    // ======== 棰勭害 ========
    // 杈撳叆: userId, bookId; status 杩斿洖缁撴灉
    // 杈撳嚭: 棰勭害璁板綍锛堝け璐ユ椂 id=0锛?
    Reservation reserveBook(int userId, int bookId, Status& status);
    Status cancelReservation(int reservationId);
    vector<Reservation> getMyReservations(int userId);

    // ======== 绠＄悊 ========
    Status addBook(const Book& book);
    Status updateBook(const Book& book);
    Status deleteBook(int bookId, int adminId);
    vector<User> getAllUsers();
    Status disableUser(int userId, int adminId);
    Status enableUser(int userId);

    // ======== 缁熻 ========
    // done: false=鎵ц涓?姹囨€诲琛?, true=宸茬粨鏉?
    struct Stats { int totalBooks; int totalUsers; int activeBorrows;
                   int overdueCount; double totalFines; };
    Stats getStatistics(Status& status, bool& done);

private:
    ApiBridge();                                    // 绂佹澶栭儴鏋勯€?
    ~ApiBridge();
    ApiBridge(const ApiBridge&) = delete;           // 绂佹鎷疯礉
    ApiBridge& operator=(const ApiBridge&) = delete;

    // 鈥斺€?鍚?Service 瀹炰緥 鈥斺€?
    unique_ptr<AuthService>        authSvc_;
    unique_ptr<BookService>        bookSvc_;
    unique_ptr<BorrowService>      borrowSvc_;
    unique_ptr<ReservationService> resSvc_;
    unique_ptr<OverdueScanner>     overdueScanner_;
    unique_ptr<LogService>         logSvc_;

    User currentUser_;             // 褰撳墠鐧诲綍鐢ㄦ埛
    bool initialized_ = false;

    // -- Internal: DAOs --
    class DatabaseManager* db_ = nullptr;
    class UserDao*        userDao_       = nullptr;
    class BookDao*        bookDao_       = nullptr;
    class BorrowRecordDao* brDao_        = nullptr;
    class CategoryDao*    catDao_        = nullptr;
    class ReservationDao* resDao_        = nullptr;
    class LogDao*         logDao_        = nullptr;

    // -- Internal: Algorithm modules --
    class SearchIndex*       searchIdx_     = nullptr;
    class RecommendEngine*   recommendEng_  = nullptr;
    class CategoryTree*      categoryTree_  = nullptr;
    class CreditEngine*      creditEng_     = nullptr;
    class FineCalculator*    fineCalc_      = nullptr;
    class ReservationQueue*  resQueue_      = nullptr;

    class OverdueScanner*    scanner_       = nullptr;
};


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 绗洓閮ㄥ垎锛氫笟鍔℃湇鍔″眰 鈥?浣樻椂瑁?                                   鈺?
// 鈺? 浜嬪姟鎺у埗 + 骞跺彂瀹夊叏 + 瀹氭椂浠诲姟                                    鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

// ----------------------------------------------------------------
//  S1. AuthService 鈥?璁よ瘉鏈嶅姟
// ----------------------------------------------------------------
class AuthService {
private:
    class UserDao* userDao_;

public:
    explicit AuthService(class UserDao* dao);

    // 杈撳叆: 鐢ㄦ埛鍚? 鏄庢枃瀵嗙爜
    // 杈撳嚭: 鐢ㄦ埛瀵硅薄锛堝け璐ユ椂 id=0锛? status.code 鍚敊璇爜, status.msg 鍚腑鏂囦俊鎭?
    User login(const string& username, const string& password, Status& status);

    // 杈撳叆: 鐢ㄦ埛鍚? 鏄庢枃瀵嗙爜, 濮撳悕, 瑙掕壊
    // 杈撳嚭: 鐢ㄦ埛瀵硅薄锛堝け璐ユ椂 id=0锛? status 鍚敞鍐岀粨鏋?
    User registerUser(const string& username, const string& password,
                      const string& realName, const string& role, Status& status);
};


// ----------------------------------------------------------------
//  S2. BookService 鈥?鍥句功鏈嶅姟
// ----------------------------------------------------------------
class BookService {
private:
    class BookDao*     bookDao_;
    class LogService*  logSvc_;
    class SearchIndex*       searchIdx_;
    class RecommendEngine*   recommendEng_;
    class CategoryTree*      categoryTree_;

public:
    BookService(class BookDao* bDao, class LogService* lSvc,
    class SearchIndex* idx, RecommendEngine* rec, CategoryTree* cat);

    // done: false=鎵ц涓? true=宸茬粨鏉?
    pair<vector<Book>, int> search(const string& query, int categoryId,
                                   int page, int pageSize,
                                   Status& status, bool& done);

    Book getDetail(int bookId);

    vector<Book> getRecommendations(int userId, int topN, Status& status, bool& done);

    // 绠＄悊鎿嶄綔锛堥渶瑕?adminId 璁板綍鏃ュ織锛? status 杩斿洖缁撴灉
    Status addBook(const Book& book, int adminId);
    Status updateBook(const Book& book, int adminId);
    Status deleteBook(int bookId, int adminId);
};


// ----------------------------------------------------------------
//  S3. BorrowService锛堟牳蹇冿級鈥?鍊熼槄鏈嶅姟 + 涔愯閿佷簨鍔?
// ----------------------------------------------------------------
class BorrowService {
private:
    class BorrowRecordDao* borrowDao_;
    class BookDao*         bookDao_;
    class CreditEngine*          creditEng_;
    class ReservationQueue*      resQueue_;

    static constexpr int MAX_RENEW       = 2;       // 鏈€澶х画鍊熸鏁?
    static constexpr int RENEW_DAYS      = 15;      // 缁€熷欢闀垮ぉ鏁?
    static constexpr int BORROW_DAYS     = 30;      // 鍊熼槄澶╂暟
    static constexpr int MAX_RETRY       = 3;       // 涔愯閿佹渶澶ч噸璇曟鏁?

public:
    BorrowService(class BorrowRecordDao* brDao, class BookDao* bDao,
    class CreditEngine* credit, ReservationQueue* queue);

    // done: false=鎵ц涓?鍚箰瑙傞攣閲嶈瘯), true=宸茬粨鏉?
    BorrowRecord borrowBook(int userId, int bookId, Status& status, bool& done);
    BorrowRecord returnBook(int borrowId, Status& status, bool& done);
    BorrowRecord renewBook(int borrowId, Status& status, bool& done);

    // 杈撳叆: userId; 杈撳嚭: 鍊熼槄璁板綍鍒楄〃
    vector<BorrowRecord> getMyRecords(int userId);
};


// ----------------------------------------------------------------
//  S4. ReservationService 鈥?棰勭害鏈嶅姟
// ----------------------------------------------------------------
class ReservationService {
private:
    class ReservationDao* reserveDao_;
    class ReservationQueue*     queue_;

public:
    ReservationService(class ReservationDao* dao, ReservationQueue* q);

    // 杈撳叆: userId, bookId; status 杩斿洖缁撴灉
    // 杈撳嚭: 棰勭害璁板綍锛堝け璐ユ椂 id=0, status.msg 鍚師鍥狅級
    Reservation reserveBook(int userId, int bookId, Status& status);

    // 杈撳叆: reservationId
    // 杈撳嚭: Status 瀵硅薄锛屽寘鍚彇娑堢粨鏋?
    Status cancelReservation(int reservationId);

    // 杈撳叆: userId; 杈撳嚭: 棰勭害鍒楄〃
    vector<Reservation> getMyReservations(int userId);

    // 褰掕繕瑙﹀彂: 閫氱煡闃熼鐢ㄦ埛锛堝唴閮ㄨ皟鐢?queue_.dequeue()锛?
    void processReturn(int bookId);

    // 瀹氭椂鎵弿: 澶勭悊瓒呮椂棰勭害锛堝唴閮ㄨ皟鐢?queue_.processExpired()锛?
    void processExpired();
};


// ----------------------------------------------------------------
//  S5. OverdueScanner 鈥?閫炬湡鎵弿瀹氭椂浠诲姟
// ----------------------------------------------------------------
class OverdueScanner {
private:
    class BorrowRecordDao* borrowDao_;
    class BookDao*         bookDao_;
    class CreditEngine*          creditEng_;

public:
    OverdueScanner(class BorrowRecordDao* brDao, class BookDao* bDao,
    class CreditEngine* credit);

    // 鎵ц涓€娆℃壂鎻? 鎵惧埌鎵€鏈夐€炬湡璁板綍 鈫?鏇存柊鐘舵€?鈫?璁＄畻缃氭 鈫?鎵ｄ俊鐢ㄥ垎
    void scan();
};


// ----------------------------------------------------------------
//  S6. LogService 鈥?瀹¤鏃ュ織鏈嶅姟
// ----------------------------------------------------------------
class LogService {
private:
    class LogDao* logDao_;

public:
    explicit LogService(class LogDao* dao);

    // 杈撳叆: 绠＄悊鍛業D, 鎿嶄綔绫诲瀷, 鎿嶄綔瀵硅薄, 璇︽儏
    void log(int adminId, const string& action,
             const string& target, const string& detail);

    // 杈撳叆: 绠＄悊鍛業D; 杈撳嚭: 璇ョ鐞嗗憳鐨勫叏閮ㄦ搷浣滄棩蹇?
    vector<LogEntry> getAdminLogs(int adminId);

    // 杈撳叆: 澶╂暟; 杈撳嚭: 鏈€杩慛澶╃殑鍏ㄩ儴鎿嶄綔鏃ュ織
    vector<LogEntry> getRecentLogs(int days);
};


// 鈺斺晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晽
// 鈺? 绗簲閮ㄥ垎锛氭暟鎹寔涔呭眰 (DAO) 鈥?浣樻椂瑁?                             鈺?
// 鈺? SQLite + 6寮犺〃 + 绱㈠紩 + 瑙﹀彂鍣?                                 鈺?
// 鈺氣晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨暆

// ----------------------------------------------------------------
//  D1. DatabaseManager 鈥?鏁版嵁搴撶鐞嗗櫒
// ----------------------------------------------------------------
class DatabaseManager {
private:
    void* db_;           // sqlite3* 鎸囬拡

    void createTables();
    void createIndexes();
    void createTriggers();

public:
    DatabaseManager();
    ~DatabaseManager();

    // 杈撳叆: 鏁版嵁搴撴枃浠惰矾寰?
    // 杈撳嚭: Status, code=OK琛ㄧず鎴愬姛锛宮sg鍚叿浣撻敊璇?
    Status open(const string& dbPath = "library.db");
    void close();

    // 浜嬪姟鎺у埗
    Status beginTransaction();
    Status commit();
    Status rollback();

    // 杈撳叆: SQL 璇彞; 杈撳嚭: 鎵ц鐘舵€?
    Status execute(const string& sql);

    // 杈撳嚭: 搴曞眰 sqlite3* 鍙ユ焺锛堜緵 DAO 浣跨敤锛?
    void* getHandle();
};


// ----------------------------------------------------------------
//  D2~D7: 鍚?DAO 绫伙紙灏佽 SQL 鎿嶄綔锛屽澶栨毚闇蹭笟鍔℃帴鍙ｏ級
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
    User insert(const User& user, Status& status);       // 杩斿洖甯d鐨刄ser
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
    Book insert(const Book& book, Status& status);       // 杩斿洖甯d鐨凚ook
    Status update(const Book& book);
    Status deleteBook(int bookId);

    // 涔愯閿佹洿鏂板簱瀛? 杈撳叆 delta锛?1/-1锛? 鏈熸湜鐗堟湰鍙?
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

