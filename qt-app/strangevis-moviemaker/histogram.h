#pragma once
#include <QtCharts>
#include <QColor>
#include <QVector>
#include <Qlist>
#include "layer.h"
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

	void showHovering(bool status, int index);
	void registerClick(int index);

	QVector<int> interval;

	bool endClick = false;
	QGraphicsRectItem clickItem;
	QGraphicsRectItem hoverItem;

	Renderer* m_renderer;

public slots:
	void updatePhaseFunction(int start, int end, QVector<float>);
	void updateLayers(QList<Layer*>);
};

