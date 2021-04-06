#pragma once
#include <QtCharts>
#include "model.h"

class Histogram {
public:
	QChartView* getHistogram();
	Histogram(QVector<unsigned short>);
private:
	QChartView* chartView;
};