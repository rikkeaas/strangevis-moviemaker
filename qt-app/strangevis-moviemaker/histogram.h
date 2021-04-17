#pragma once
#include <QtCharts>
#include <QColor>
#include <QVector>
#include "model.h"
#include "Renderer.h"

class Histogram : public QWidget {
public:
	QChartView* getHistogram();
	Histogram(Renderer* renderer);

private:
	std::map<float, int> binData(QVector<unsigned short>, int, int);
	QChartView* chartViewP;
	float roundNearest(int, float);

	void showHovering(bool status, int index);
	void registerClick(int index);

	QVector<int> interval;

	bool endClick = false;
	QGraphicsRectItem clickItem;
	QGraphicsRectItem hoverItem;

	Renderer* m_renderer;
};

