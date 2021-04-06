#include "histogram.h"
#include "model.h"
#include <cfloat>


Histogram::Histogram(QVector<unsigned short> values) {
    QBarSet* barChart = new QBarSet("Density");
    barChart->setBorderColor(Qt::white);
    barChart->setColor(Qt::white);
    QStringList categories;

    if (!values.isEmpty()) {
        for (int i = 0; i < 100; i++) {
            float val = values[i];
            *barChart << val;
            QString str = "d";
            str.append(i);
            categories << str;
        }
    }

    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);

    QChart* chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->legend()->setVisible(false);
    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(false);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

QChartView* Histogram::getHistogram()
{
	return chartView;
}