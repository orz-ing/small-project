#include "admin_dashboard.h"
#include "bridge/api_bridge.h"
#include "dao/database_manager.h"
#include "dao/book_dao.h"
#include "dao/user_dao.h"
#include "tools/mock_generator.h"
#include "tools/csv_exporter.h"
#include "tools/json_exporter.h"
#include "utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDatabase>

AdminDashboard::AdminDashboard(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void AdminDashboard::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📊 管理仪表盘");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* btnLayout = new QHBoxLayout;
    m_genDataBtn = new QPushButton("⚡ 生成演示数据");
    m_genDataBtn->setObjectName("warningBtn");
    connect(m_genDataBtn, &QPushButton::clicked, this, &AdminDashboard::generateDemoData);
    btnLayout->addWidget(m_genDataBtn);

    auto* csvBtn = new QPushButton("📄 导出CSV");
    csvBtn->setObjectName("successBtn");
    connect(csvBtn, &QPushButton::clicked, this, &AdminDashboard::exportCsv);
    btnLayout->addWidget(csvBtn);

    auto* jsonBtn = new QPushButton("📋 导出JSON");
    jsonBtn->setObjectName("primaryBtn");
    connect(jsonBtn, &QPushButton::clicked, this, &AdminDashboard::exportJson);
    btnLayout->addWidget(jsonBtn);

    m_importCsvBtn = new QPushButton("📥 导入CSV");
    m_importCsvBtn->setObjectName("primaryBtn");
    connect(m_importCsvBtn, &QPushButton::clicked, this, &AdminDashboard::importCsv);
    btnLayout->addWidget(m_importCsvBtn);

    m_importJsonBtn = new QPushButton("📥 导入JSON");
    m_importJsonBtn->setObjectName("successBtn");
    connect(m_importJsonBtn, &QPushButton::clicked, this, &AdminDashboard::importJson);
    btnLayout->addWidget(m_importJsonBtn);

    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    auto* refreshBtn = new QPushButton("🔄 刷新数据");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &AdminDashboard::refreshData);
    layout->addWidget(refreshBtn);
    layout->addSpacing(10);

    auto* grid = new QGridLayout;
    grid->setSpacing(15);

    auto createCard = [](const QString& icon, const QString& label, QLabel*& valueLabel) -> QFrame* {
        auto* card = new QFrame;
        card->setObjectName("card");
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setAlignment(Qt::AlignCenter);
        auto* iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet("font-size: 28px;");
        iconLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(iconLabel);
        valueLabel = new QLabel("0");
        valueLabel->setObjectName("statValue");
        valueLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(valueLabel);
        auto* descLabel = new QLabel(label);
        descLabel->setObjectName("statLabel");
        descLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(descLabel);
        return card;
    };

    grid->addWidget(createCard("📖", "图书总量", m_totalBooks), 0, 0);
    grid->addWidget(createCard("👥", "用户总量", m_totalUsers), 0, 1);
    grid->addWidget(createCard("📚", "当前借阅", m_activeBorrows), 0, 2);
    grid->addWidget(createCard("⚠️", "逾期数量", m_overdueCount), 1, 0);
    grid->addWidget(createCard("💰", "总罚款(元)", m_totalFine), 1, 1);
    grid->addWidget(createCard("📅", "今日借阅", m_todayBorrows), 1, 2);

    layout->addLayout(grid);
    layout->addStretch();
    refreshData();
}

void AdminDashboard::generateDemoData() {
    QSqlDatabase db = DatabaseManager::instance().database();
    MockGenerator::generateToDB(db, 30, 8, 25, 8, 10);

    refreshData();
    ApiBridge::instance()->rebuildIndex();
    QMessageBox::information(this, "成功", "随机演示数据生成完毕！\n\n管理员: admin/admin123\n读者: user1~5/123456");
}

void AdminDashboard::exportCsv() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择导出目录");
    if (dir.isEmpty()) return;

    auto* bridge = ApiBridge::instance();
    auto allBooks = bridge->getAllBooks();
    auto allUsers = bridge->getAllUsers();
    QSqlDatabase db = DatabaseManager::instance().database();
    BorrowRecordDAO borrowDao(db);
    auto allRecords = borrowDao.getAll();

    int ok = 0;
    if (CsvExporter::exportBooks(dir + "/books.csv", allBooks)) ok++;
    if (CsvExporter::exportUsers(dir + "/users.csv", allUsers)) ok++;
    if (CsvExporter::exportBorrowRecords(dir + "/borrow_records.csv", allRecords)) ok++;

    QMessageBox::information(this, "导出完成",
        QString("CSV文件已导出到：%1/\n\n成功导出 %2 个文件").arg(dir).arg(ok));
}

void AdminDashboard::exportJson() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择导出目录");
    if (dir.isEmpty()) return;

    auto* bridge = ApiBridge::instance();
    auto allBooks = bridge->getAllBooks();
    auto stats = bridge->getStatistics();
    QSqlDatabase db = DatabaseManager::instance().database();
    BorrowRecordDAO borrowDao(db);
    auto allRecords = borrowDao.getAll();

    int ok = 0;
    if (JsonExporter::exportBooks(dir + "/books.json", allBooks)) ok++;
    if (JsonExporter::exportBorrowRecords(dir + "/borrow_records.json", allRecords)) ok++;
    if (JsonExporter::exportStatistics(dir + "/statistics.json", stats)) ok++;

    QMessageBox::information(this, "导出完成",
        QString("JSON文件已导出到：%1/\n\n成功导出 %2 个文件").arg(dir).arg(ok));
}

void AdminDashboard::importCsv() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择CSV文件", QString(), "CSV文件 (*.csv)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "失败", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // 读取第一行（表头）
    QString header = in.readLine();
    if (header.isEmpty()) { file.close(); return; }

    // 判断数据类型
    bool isBooks = header.contains("书名") || header.contains("ID,书名");
    bool isUsers = header.contains("用户名");
    bool isRecords = header.contains("借阅日期");

    int imported = 0;
    QString line;

    if (isBooks) {
        BookDAO bookDao(DatabaseManager::instance().database());
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            QStringList parts = line.split(",");
            if (parts.size() < 2) continue;

            Book book;
            int col = 1; // 跳过ID列
            if (col < parts.size()) book.title = parts.value(col).trimmed().remove('"');
            col++;
            if (col < parts.size()) book.author = parts.value(col).trimmed().remove('"');
            col++;
            if (col < parts.size()) book.isbn = parts.value(col).trimmed();
            col++;
            if (col < parts.size()) book.publisher = parts.value(col).trimmed().remove('"');
            col++;
            if (col < parts.size()) book.categoryPath = parts.value(col).trimmed();
            col++;
            if (col < parts.size()) book.totalStock = parts.value(col).toInt();
            book.availableStock = book.totalStock;

            int id = bookDao.insert(book);
            if (id > 0) imported++;
        }
    } else if (isUsers) {
        UserDAO userDao(DatabaseManager::instance().database());
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            QStringList parts = line.split(",");
            if (parts.size() < 2) continue;

            User user;
            user.username = parts.value(1).trimmed();
            user.displayName = parts.value(2).trimmed().remove('"');
            user.creditScore = 100;
            user.passwordHash = Utils::sha256("123456");

            int id = userDao.insert(user);
            if (id > 0) imported++;
        }
    } else if (isRecords) {
        QMessageBox::information(this, "提示", "借阅记录导入暂不支持，请使用JSON格式导入");
    }

    file.close();

    if (imported > 0) {
        ApiBridge::instance()->rebuildIndex();
        QMessageBox::information(this, "导入成功",
            QString("成功导入 %1 条记录").arg(imported));
        refreshData();
    } else {
        QMessageBox::information(this, "导入完成", "未导入任何数据，请确认文件格式正确");
    }
}

void AdminDashboard::importJson() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择JSON文件", QString(), "JSON文件 (*.json)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "失败", "无法打开文件");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        QMessageBox::warning(this, "失败", "JSON格式错误，需要JSON数组格式");
        return;
    }

    QJsonArray arr = doc.array();
    int imported = 0;

    if (arr.isEmpty()) return;
    QJsonObject first = arr.first().toObject();

    bool isBooks = first.contains("title") || first.contains("isbn");
    bool isUsers = first.contains("username") || first.contains("passwordHash");
    bool isRecords = first.contains("borrowDate") || first.contains("bookId");

    if (isBooks) {
        BookDAO bookDao(DatabaseManager::instance().database());
        for (const auto& val : arr) {
            Book book = Book::fromJson(val.toObject());
            book.availableStock = book.totalStock;
            int id = bookDao.insert(book);
            if (id > 0) imported++;
        }
    } else if (isUsers) {
        UserDAO userDao(DatabaseManager::instance().database());
        for (const auto& val : arr) {
            User user = User::fromJson(val.toObject());
            if (user.passwordHash.isEmpty())
                user.passwordHash = Utils::sha256("123456");
            int id = userDao.insert(user);
            if (id > 0) imported++;
        }
    }

    if (imported > 0) {
        ApiBridge::instance()->rebuildIndex();
        QMessageBox::information(this, "导入成功",
            QString("成功导入 %1 条记录").arg(imported));
        refreshData();
    } else {
        QMessageBox::information(this, "导入完成", "未导入任何数据");
    }
}

void AdminDashboard::refreshData() {
    auto stats = ApiBridge::instance()->getStatistics();
    m_totalBooks->setText(QString::number(stats.totalBooks));
    m_totalUsers->setText(QString::number(stats.totalUsers));
    m_activeBorrows->setText(QString::number(stats.activeBorrows));
    m_overdueCount->setText(QString::number(stats.overdueCount));
    m_totalFine->setText(QString::number(stats.totalFine, 'f', 1));
    m_todayBorrows->setText(QString::number(stats.todayBorrows));
}
