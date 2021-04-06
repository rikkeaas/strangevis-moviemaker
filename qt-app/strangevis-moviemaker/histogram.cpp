#include "histogram.h"
#include "model.h"
#include <cfloat>


Histogram::Histogram(QVector<unsigned short> values) {
    QBarSet* barChart = new QBarSet("Density");
    QStringList categories;

    float minVal = FLT_MAX;
    float maxVal = 0;
    if (!values.isEmpty()) {
        for (int i = 0; i < 100; i++) {
            float val = values[i];
            *barChart << val;
            QString str = "d";
            str.append(i);

            categories << str;
            if (val > maxVal) {
                maxVal = val;
            }
            else if (val < minVal) {
                minVal = val;
            }
        }
    }

    QBarSeries* series = new QBarSeries();
    series->append(barChart);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Example");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, int(maxVal*1.01));
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