#include "histogram.h"
#include "model.h"
#include <cfloat>
using namespace std;


Histogram::Histogram(QVector<unsigned short> values) {
    QBarSet* barChart = new QBarSet("Density");
    barChart->setBorderColor(Qt::white);
    barChart->setColor(Qt::white);
    QStringList categories;

    if (!values.isEmpty()) {
        int showValuesAbove = 100;
        int skipStep = 50;
        int roundTo = 10;

        std::map<float, int> bin = binData(values, skipStep, roundTo);
        int s = bin.size();
        // qDebug() << "Map size: " << s;
        // qDebug() << "Map content: " << bin;

        int skipped = 0;
        std::map<float, int>::iterator itr;
        itr = bin.begin();


        for (int i = 0; i < s; i++) {
            float key = itr->first;
            // qDebug() << "Key: " << key << " Value: " << bin.at(key);
            int val = bin.at(key);
            ++itr;
            if (val < showValuesAbove) {
                skipped += 1;
                continue;
            }
            *barChart << val;
            QString str = "d";
            str.append(i);
            categories << str;
        }
        qDebug() << "Vals: " << (s-skipped);
    }

    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);

    QChart* chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->legend()->setVisible(false);
    // chart->setBackgroundVisible(false);
    chart->setBackgroundBrush(QColor(92,91,91));
    chart->setPlotAreaBackgroundVisible(false);
    chart->setBackgroundRoundness(15);
    chart->setMaximumHeight(QDesktopWidget().availableGeometry().height() * 0.20);
    // chart->setMargins({ 0, 0, 0, 0 });
    chart->setContentsMargins(0, 0, 0, 0);
    chart->layout()->setContentsMargins(0, 0, 0, 0);


    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);


}

std::map<float, int> Histogram::binData(QVector<unsigned short> values, int skipStep, int roundTo) {
    std::map<float, int> occurences;
    for (int i = 0; i < values.size(); i += skipStep) {
        float val = roundNearest(roundTo, values[i]);
        if (occurences.count(val) > 0) {
            occurences[val] += 1;
        }
        else {
            occurences.insert(make_pair(val, 1));
        }
    }
    return occurences;
}

QChartView* Histogram::getHistogram()
{
	return chartView;
}

float Histogram::roundNearest(int roundTo, float d)
{
    int r = roundTo;
    int d_i = d;
    return ((d_i % r) < (r/2)) ? d_i - (d_i % r) : d_i + (r - (d_i % r));
}

