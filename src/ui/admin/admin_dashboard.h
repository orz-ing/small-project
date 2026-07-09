#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

class AdminDashboard : public QWidget {
    Q_OBJECT
public:
    explicit AdminDashboard(QWidget* parent = nullptr);
public:
    void refreshData();
    void generateDemoData();
    void exportCsv();
    void exportJson();
    void importCsv();
    void importJson();
private:
    void setupUI();
    QLabel* m_totalBooks;
    QLabel* m_totalUsers;
    QLabel* m_activeBorrows;
    QLabel* m_overdueCount;
    QLabel* m_totalFine;
    QLabel* m_todayBorrows;
    QPushButton* m_genDataBtn;
    QPushButton* m_importCsvBtn;
    QPushButton* m_importJsonBtn;
};
#endif
