#include "histogram.h"
#include "model.h"


Histogram::Histogram(QVector<unsigned short> values) {
    QBarSet* barChart = new QBarSet("Density");
    qDebug() << "Values: " << values.isEmpty();

    if (!values.isEmpty()) {
        for (int i = 0; i < 100; i++) {
            qDebug() << "m_Data" << i << ": " << values[i];
            *barChart << values[i];
        }
    }
    

    // std::list<float> values;
    // values.push_back(10);
    // for (float f : values)
    //     *barChart << f;
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

    double max = *std::max_element(values.constBegin(), values.constEnd());
    axisY->setRange(0, max);
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