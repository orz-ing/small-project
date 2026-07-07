/*
#ifndef STATISTICS_WIDGET_H
#define STATISTICS_WIDGET_H

#include <QWidget>

class StatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
private:
    void setupUI();
};

#endif
*/
#ifndef STATISTICS_WIDGET_H
#define STATISTICS_WIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>      // 关键：使用 QtCharts/ 前缀
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>
#include <QtCharts/QLineSeries>

// 不要使用任何 using namespace，直接用类名
class StatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

private:
    void setupUI();
    QChartView* createBarChart();
    QChartView* createPieChart();
    QChartView* createLineChart();
};

#endif