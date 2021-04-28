#pragma once
#include <QWidget>
#include <QVector4D>
#include "layer.h"
#include "histogramChartView.h"
#include <QList>

class LayerHandler : public QWidget
{
	Q_OBJECT
public:
	LayerHandler(HistogramChartView* chartView);
private:
	Layer* m_selectedLayer;
	QList<Layer*>  m_layers;
	QVector<float> m_phaseFuncData;
	void updatePhaseFuncData();
	HistogramChartView* m_chartView;
public slots:
	void addLayer(QRect area);
	void layerSelected(Layer* selectedLayer, bool remove);

signals:
	void displayLayer(QRect area);
	void undisplayLayer(QRect area);
	void updateRedSlider(int red);
	void updateGreenSlider(int green);
	void updateBlueSlider(int blue);
	void updateAlphaSlider(int alpha);
	void updatePhaseFunction(int start, int end, QVector<float> data);
};