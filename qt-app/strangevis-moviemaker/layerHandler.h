#pragma once
#include <QWidget>
#include <QVector4D>
#include "layer.h"
#include <QList>

class LayerHandler : public QWidget
{
	Q_OBJECT
public:
	LayerHandler();
private:
	Layer* m_selectedLayer;
	QList<Layer*>  m_layers;
	int minX, maxX;
	QVector<float> m_phaseFuncData;
	void updatePhaseFuncData();
public slots:
	void addLayer(QRect area);
	void layerSelected(Layer* selectedLayer, bool remove);
	void redChanged(int);
	void greenChanged(int);
	void blueChanged(int);
	void alphaChanged(int);
signals:
	void displayLayer(QRect area);
	void undisplayLayer(QRect area);
	void updateRedSlider(int red);
	void updateGreenSlider(int green);
	void updateBlueSlider(int blue);
	void updateAlphaSlider(int alpha);
	void updatePhaseFunction(int start, int end, QVector<float> data);
};