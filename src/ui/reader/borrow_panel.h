#ifndef BORROW_PANEL_H
#define BORROW_PANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>

class BorrowPanel : public QWidget {
    Q_OBJECT
public:
    explicit BorrowPanel(QWidget* parent = nullptr);
    void refreshData();
private slots:
    void onReturnBook(int recordId);
    void onRenewBook(int recordId);
private:
    void setupUI();
    QTableWidget* m_activeTable;
    QTableWidget* m_historyTable;
    QTabWidget* m_tabWidget;
    QLabel* m_summaryLabel;
};
#endif
