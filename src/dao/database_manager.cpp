#include "database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::initialize(const QString& dbPath) {
    if (m_initialized) return true;

    m_dbPath = dbPath;

    // 确保目录存在
    QDir dir = QFileInfo(m_dbPath).absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    // 启用WAL模式，支持并发读
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode=WAL");
    query.exec("PRAGMA foreign_keys=ON");

    if (!createTables()) {
        qCritical() << "Failed to create tables";
        return false;
    }

    m_initialized = true;
    qInfo() << "Database initialized:" << m_dbPath;
    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return QSqlDatabase::database("main_connection");
}

void DatabaseManager::close() {
    if (m_initialized) {
        QSqlDatabase::database("main_connection").close();
        QSqlDatabase::removeDatabase("main_connection");
        m_initialized = false;
    }
}

bool DatabaseManager::createTables() {
    QSqlDatabase db = database();
    QSqlQuery query(db);

    // 分类表
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS categories ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  parent_id INTEGER DEFAULT -1,"
        "  name TEXT NOT NULL,"
        "  path TEXT DEFAULT '',"
        "  level INTEGER DEFAULT 0"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create categories table:" << query.lastError().text();
        return false;
    }

    // 图书表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  isbn TEXT UNIQUE,"
        "  title TEXT NOT NULL,"
        "  author TEXT DEFAULT '',"
        "  publisher TEXT DEFAULT '',"
        "  category_id INTEGER DEFAULT 0,"
        "  category_path TEXT DEFAULT '',"
        "  total_stock INTEGER DEFAULT 1,"
        "  available_stock INTEGER DEFAULT 1,"
        "  version INTEGER DEFAULT 0,"
        "  cover_path TEXT DEFAULT '',"
        "  description TEXT DEFAULT '',"
        "  create_time TEXT DEFAULT (datetime('now','localtime')),"
        "  update_time TEXT DEFAULT (datetime('now','localtime')),"
        "  FOREIGN KEY (category_id) REFERENCES categories(id)"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create books table:" << query.lastError().text();
        return false;
    }

    // 用户表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password_hash TEXT NOT NULL,"
        "  display_name TEXT DEFAULT '',"
        "  role INTEGER DEFAULT 1,"  // 0=Admin, 1=Reader
        "  credit_score INTEGER DEFAULT 100,"
        "  is_disabled INTEGER DEFAULT 0,"
        "  email TEXT DEFAULT '',"
        "  phone TEXT DEFAULT '',"
        "  create_time TEXT DEFAULT (datetime('now','localtime'))"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    // 借阅记录表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS borrow_records ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id INTEGER NOT NULL,"
        "  book_id INTEGER NOT NULL,"
        "  book_title TEXT DEFAULT '',"
        "  book_isbn TEXT DEFAULT '',"
        "  borrow_date TEXT DEFAULT (datetime('now','localtime')),"
        "  due_date TEXT NOT NULL,"
        "  return_date TEXT,"
        "  fine REAL DEFAULT 0.0,"
        "  status INTEGER DEFAULT 0,"  // 0=Borrowing, 1=Returned, 2=Overdue, 3=Lost
        "  FOREIGN KEY (user_id) REFERENCES users(id),"
        "  FOREIGN KEY (book_id) REFERENCES books(id)"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create borrow_records table:" << query.lastError().text();
        return false;
    }

    // 预约表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS reservations ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id INTEGER NOT NULL,"
        "  book_id INTEGER NOT NULL,"
        "  book_title TEXT DEFAULT '',"
        "  reserve_date TEXT DEFAULT (datetime('now','localtime')),"
        "  expire_date TEXT,"
        "  priority INTEGER DEFAULT 0,"
        "  status INTEGER DEFAULT 0,"  // 0=Pending, 1=Notified, 2=Cancelled, 3=Expired, 4=Fulfilled
        "  FOREIGN KEY (user_id) REFERENCES users(id),"
        "  FOREIGN KEY (book_id) REFERENCES books(id)"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create reservations table:" << query.lastError().text();
        return false;
    }

    // 日志表
    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS logs ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  admin_id INTEGER DEFAULT 0,"
        "  admin_name TEXT DEFAULT '',"
        "  action TEXT NOT NULL,"
        "  target_type TEXT DEFAULT '',"
        "  target_id INTEGER DEFAULT 0,"
        "  detail TEXT DEFAULT '',"
        "  timestamp TEXT DEFAULT (datetime('now','localtime'))"
        ")"
    );
    if (!ok) {
        qCritical() << "Failed to create logs table:" << query.lastError().text();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_books_category ON books(category_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_books_title ON books(title)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_borrow_user ON borrow_records(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_borrow_book ON borrow_records(book_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_borrow_status ON borrow_records(status)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_reserve_user ON reservations(user_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_reserve_book ON reservations(book_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_logs_action ON logs(action)");

    return true;
}
