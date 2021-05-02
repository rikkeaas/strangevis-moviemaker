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
	QList<Layer*> getLayers();
private:
	Layer* m_selectedLayer;
	QList<Layer*>  m_layers;
	QVector<float> m_transferFuncData;
	void updateTransferFuncData();
	HistogramChartView* m_chartView;
public slots:
	void addLayer(QRect area);
	void layerSelected(Layer* selectedLayer, bool remove);
	void setLayers(QList<Layer*> layers);
signals:
	void displayLayer(QRect area);
	void undisplayLayer(QRect area);
	void updateRedSlider(int red);
	void updateGreenSlider(int green);
	void updateBlueSlider(int blue);
	void updateAlphaSlider(int alpha);
	void updateTransferFunction(int start, int end, QVector<float> data);
};