#ifndef STATISTICS_WIDGET_H
#define STATISTICS_WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>

class StatisticsWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget* parent = nullptr);
    void refreshData();
private:
    void setupUI();
    void setupBarChart();
    void setupPieChart();
    void setupLineChart();

    QTabWidget* m_tabWidget;
    QWidget* m_barPage;
    QWidget* m_piePage;
    QWidget* m_linePage;
};
#endif
