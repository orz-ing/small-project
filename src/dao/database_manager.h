#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initialize(const QString& dbPath = "library.db");
    QSqlDatabase database() const;
    void close();
    bool isInitialized() const { return m_initialized; }

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();

    QString m_dbPath;
    bool m_initialized = false;
};

#endif // DATABASE_MANAGER_H
