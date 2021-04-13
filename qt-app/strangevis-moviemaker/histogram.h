#pragma once
#include <QtCharts>
#include <QColor>
#include "model.h"

class Histogram {
public:
	QChartView* getHistogram();
	Histogram(QVector<unsigned short>*);
private:
	std::map<float, int> binData(QVector<unsigned short>*, int, int);
	QChartView* chartView;
	float roundNearest(int, float);
};