#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
class search_widget : public QWidget
{
    Q_OBJECT
public:
    explicit search_widget(QWidget *parent = nullptr);

private slots:
    void onSearch();

private:
    void setupUI();
    void loadMockData();          // 临时假数据
    void updateTable(const QString &keyword);

    QLineEdit   *m_searchEdit;
    QPushButton *m_searchBtn;
    QTableWidget *m_table;
    QLabel      *m_statusLabel;

    // 假数据存储（用结构体或直接 QList<QStringList>）
    QList<QStringList> m_mockBooks;  // 每行：书名, 作者, ISBN, 库存
};


#endif // SEARCH_WIDGET_H
