#include "statistics_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QPieSeries>
#include <QDateTime>
#include <QLabel>
#include <QRandomGenerator>
StatisticsWidget::StatisticsWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void StatisticsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("数据概览");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; padding: 10px;");
    mainLayout->addWidget(title);

    // 第一行：柱状图
    mainLayout->addWidget(createBarChart());

    // 第二行：饼图和折线图并排
    QHBoxLayout *bottomCharts = new QHBoxLayout;
    bottomCharts->addWidget(createPieChart());
    bottomCharts->addWidget(createLineChart());
    mainLayout->addLayout(bottomCharts);
}

QChartView* StatisticsWidget::createBarChart()
{
    QBarSet *setBooks = new QBarSet("图书数量");
    *setBooks << 120 << 85 << 65 << 90 << 50;

    QBarSeries *series = new QBarSeries;
    series->append(setBooks);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("各分类图书数量");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories = {"计算机", "文学", "历史", "科学", "艺术"};
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 150);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

QChartView* StatisticsWidget::createPieChart()
{
    QPieSeries *series = new QPieSeries;
    series->append("在馆", 60);
    series->append("借出", 30);
    series->append("预约", 10);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("图书状态分布");
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

QChartView* StatisticsWidget::createLineChart()
{
    QLineSeries *series = new QLineSeries;
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 7; ++i) {
        int num = QRandomGenerator::global()->bounded(100);
        series->append(now.addDays(-6 + i).toMSecsSinceEpoch(), 5 + num);
    }

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("近7天借阅趋势");
    chart->legend()->hide();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("MM-dd");
    axisX->setTickCount(7);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 150);   // 修改此处
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}