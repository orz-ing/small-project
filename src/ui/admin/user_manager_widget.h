#ifndef USER_MANAGER_WIDGET_H
#define USER_MANAGER_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class UserManagerWidget : public QWidget {
    Q_OBJECT
public:
    explicit UserManagerWidget(QWidget* parent = nullptr);
private slots:
    void refreshUserList();
    void onToggleDisable();
    void onSearch();
private:
    void setupUI();
    QTableWidget* m_userTable;
    QLineEdit* m_searchInput;
    QPushButton* m_toggleBtn;
};
#endif
