#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>

class SearchWidget : public QWidget {
    Q_OBJECT
public:
    explicit SearchWidget(QWidget* parent = nullptr);
private slots:
    void onSearch();
private:
    void setupUI();
    QLineEdit* m_searchInput;
    QPushButton* m_searchBtn;
    QTableWidget* m_resultTable;
    QLabel* m_resultCountLabel;
};
#endif
