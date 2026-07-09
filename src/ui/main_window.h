#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

signals:
    void loggedOut();

private slots:
    void onLogout();
    void switchPage(int index);

private:
    void setupUI();
    void setupReaderPages();
    void setupAdminPages();
    void updateNavButtons(int index);

    QWidget* m_sidebar;
    QVBoxLayout* m_navLayout;
    QVector<QPushButton*> m_navButtons;
    QLabel* m_userInfoLabel;
    QStackedWidget* m_stackedWidget;
    QVector<QWidget*> m_pages;
    QStringList m_navLabels;
    QWidget* m_readerMainPage = nullptr;
    QWidget* m_searchPage = nullptr;
    QWidget* m_adminDashboard = nullptr;
};

#endif
