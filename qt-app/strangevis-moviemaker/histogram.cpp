#include "histogram.h"


Histogram::Histogram() {
    QBarSet* barChart = new QBarSet("Density");
    *barChart << 22 << 13 << 18;
    QBarSeries* series = new QBarSeries();
    series->append(barChart);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Data";
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 25);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

}

QChartView* Histogram::getHistogram()
{
	return chartView;
}

