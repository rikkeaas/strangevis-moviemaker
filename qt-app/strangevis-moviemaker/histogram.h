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
	Histogram(Renderer* renderer, bool log, int clamp);
	LayerHandler* m_layerHandler;
	QChartView* getHistogram();
	bool animationIsPlaying();
private:
	std::map<float, int> binData(QVector<unsigned short>, int, int);
	
	float roundNearest(int, float);

	QBarSet* computeHistogram(bool log, int clamp);

	Renderer* m_renderer;
	HistogramChartView* chartView;
protected:
	void focusOutEvent(QFocusEvent* event);

public slots:
	void updateTransferFunction(int start, int end, QVector<float>);
	void updateHistogramYScaling(bool, int);
	void updateLayers();
};

