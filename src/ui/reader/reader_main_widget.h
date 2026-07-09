#ifndef READER_MAIN_WIDGET_H
#define READER_MAIN_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>

class ReaderMainWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReaderMainWidget(QWidget* parent = nullptr);
private slots:
    void onSearch();
    void onBookDoubleClicked(int row, int col);
    void onBorrowBook();
    void loadAllBooks();
private:
    void setupUI();
    void loadHotBooks();
    QLineEdit* m_searchInput;
    QPushButton* m_searchBtn;
    QTableWidget* m_resultTable;
    QTableWidget* m_hotTable;
    QLabel* m_welcomeLabel;
    int m_selectedBookId = 0;
};
#endif
