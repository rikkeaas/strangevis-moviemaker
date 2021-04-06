#pragma once
#include <QtCharts>


class Histogram {
public:
	QChartView* getHistogram();
	Histogram(void);
private:
	QChartView* chartView;
};