#include "statistics_widget.h"
#include "bridge/api_bridge.h"
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>

StatisticsWidget::StatisticsWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void StatisticsWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("📈 数据统计");
    title->setObjectName("sectionTitle");
    layout->addWidget(title);

    auto* refreshBtn = new QPushButton("🔄 刷新图表");
    refreshBtn->setObjectName("primaryBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &StatisticsWidget::refreshData);
    layout->addWidget(refreshBtn);

    m_tabWidget = new QTabWidget;

    m_barPage = new QWidget;
    m_piePage = new QWidget;
    m_linePage = new QWidget;

    m_tabWidget->addTab(m_barPage, "📊 热门图书 TOP10");
    m_tabWidget->addTab(m_piePage, "🥧 分类占比");
    m_tabWidget->addTab(m_linePage, "📈 月度借阅趋势");

    layout->addWidget(m_tabWidget, 1);

    refreshData();
}

void StatisticsWidget::refreshData() {
    auto stats = ApiBridge::instance()->getStatistics();

    // 柱状图 - 热门图书
    {
        auto* barSeries = new QBarSeries;
        auto* barSet = new QBarSet("借阅次数");
        QStringList categories;

        for (const auto& hb : stats.hotBooks) {
            *barSet << hb.borrowCount;
            categories << (hb.bookTitle.length() > 8 ? hb.bookTitle.left(8) + ".." : hb.bookTitle);
        }

        barSeries->append(barSet);

        auto* chart = new QChart;
        chart->addSeries(barSeries);
        chart->setTitle("热门图书借阅排行 TOP10");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        auto* axisX = new QBarCategoryAxis;
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        auto* axisY = new QValueAxis;
        axisY->setTitleText("借阅次数");
        axisY->setLabelFormat("%d");  // 显示整数

        // 根据数据自动调整Y轴最高值
        int maxVal = 0;
        for (const auto& hb : stats.hotBooks) {
            if (hb.borrowCount > maxVal) maxVal = hb.borrowCount;
        }
        int axisMax = qMax(maxVal + 2, 10);
        axisY->setRange(0, axisMax);
        axisY->setTickCount(qMin(axisMax + 1, 12));

        chart->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        // 替换旧视图
        auto* oldLayout = m_barPage->layout();
        if (oldLayout) {
            QLayoutItem* item;
            while ((item = oldLayout->takeAt(0))) { delete item->widget(); delete item; }
            delete oldLayout;
        }

        auto* chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        auto* newLayout = new QVBoxLayout(m_barPage);
        newLayout->addWidget(chartView);
    }

    // 饼图 - 分类占比
    {
        auto* pieSeries = new QPieSeries;

        for (const auto& cs : stats.categoryStats) {
            if (cs.bookCount > 0) {
                auto* slice = pieSeries->append(cs.categoryName, cs.bookCount);
                slice->setLabelVisible(true);
                slice->setLabel(QString("%1 (%2)").arg(cs.categoryName).arg(cs.bookCount));
            }
        }

        auto* chart = new QChart;
        chart->addSeries(pieSeries);
        chart->setTitle("图书分类占比");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // 替换旧视图
        auto* oldLayout = m_piePage->layout();
        if (oldLayout) {
            QLayoutItem* item;
            while ((item = oldLayout->takeAt(0))) { delete item->widget(); delete item; }
            delete oldLayout;
        }

        auto* chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        auto* newLayout = new QVBoxLayout(m_piePage);
        newLayout->addWidget(chartView);
    }

    // 折线图 - 月度趋势
    {
        auto* lineSeries = new QLineSeries;
        lineSeries->setName("借阅量");

        QStringList categories;
        for (const auto& ms : stats.monthlyStats) {
            QString monthStr = QString::number(ms.month);
            if (monthStr.length() >= 6) {
                categories << monthStr.mid(4, 2) + "月";
                lineSeries->append(categories.size() - 1, ms.borrowCount);
            }
        }

        auto* chart = new QChart;
        chart->addSeries(lineSeries);
        chart->setTitle("近6个月借阅趋势");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        auto* axisX = new QValueAxis;
        axisX->setRange(0, qMax(categories.size() - 1, 1));
        axisX->setTickCount(categories.size());
        axisX->setLabelFormat("%d");
        chart->addAxis(axisX, Qt::AlignBottom);
        lineSeries->attachAxis(axisX);

        auto* axisY = new QValueAxis;
        axisY->setTitleText("借阅量");
        axisY->setLabelFormat("%d");
        chart->addAxis(axisY, Qt::AlignLeft);
        lineSeries->attachAxis(axisY);

        // 替换旧视图
        auto* oldLayout = m_linePage->layout();
        if (oldLayout) {
            QLayoutItem* item;
            while ((item = oldLayout->takeAt(0))) { delete item->widget(); delete item; }
            delete oldLayout;
        }

        auto* chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        auto* newLayout = new QVBoxLayout(m_linePage);
        newLayout->addWidget(chartView);
    }
}

// Quick helper for the QT_CHARTS_USE_NAMESPACE usage
void StatisticsWidget::setupBarChart() {}
void StatisticsWidget::setupPieChart() {}
void StatisticsWidget::setupLineChart() {}
