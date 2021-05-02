#pragma once
#include <QtCharts>
#include <QColor>
#include <QVector>
#include "model.h"
#include "Renderer.h"
#include "layerHandler.h"
#include "histogramChartView.h"

class Histogram : public QWidget {
	Q_OBJECT
public:
	QChartView* getHistogram();
	Histogram(Renderer* renderer);
	LayerHandler* m_layerHandler;
private:
	std::map<float, int> binData(QVector<unsigned short>, int, int);
	HistogramChartView* chartViewP;
	float roundNearest(int, float);

	//void showHovering(bool status, int index);
	//void registerClick(int index);

	QVector<int> interval;

	bool endClick = false;

	Renderer* m_renderer;


public slots:
	void updateTransferFunction(int start, int end, QVector<float>);
};

