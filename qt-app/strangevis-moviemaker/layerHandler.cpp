#include "layerHandler.h"
#include <QDebug>
#include <QVBoxLayout>

LayerHandler::LayerHandler(HistogramChartView* chartView) : QWidget()
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setAlignment(Qt::AlignTop);
	
	setLayout(layout);
	m_selectedLayer = NULL;
	
	m_chartView = chartView;
	m_phaseFuncData.resize(256 * 4);
}

void LayerHandler::addLayer(QRect area)
{
	if (area.left() == area.right()) return;
	Layer* newLayer = new Layer(this, area);
	newLayer->setStyleSheet("background-color:#6D6D6D; height:45; border-radius:10px;");
	layout()->addWidget(newLayer);
	
	QObject::connect(newLayer, &Layer::clicked, this, &LayerHandler::layerSelected);
	QObject::connect(newLayer, &Layer::updatePhaseFunc, this, &LayerHandler::updatePhaseFuncData);

	m_layers.append(newLayer);
	layerSelected(newLayer, false);
	updatePhaseFuncData();
}

void LayerHandler::layerSelected(Layer* selectedLayer, bool remove)
{
	if (remove)
	{
		undisplayLayer(selectedLayer->m_selectedArea);
		layout()->removeWidget(selectedLayer);
		if (m_selectedLayer == selectedLayer) m_selectedLayer = NULL;
		delete selectedLayer;

	}
	else
	{
		qDebug() << selectedLayer->m_selectedArea.left() << selectedLayer->m_selectedArea.right();
		m_selectedLayer = selectedLayer;
		displayLayer(selectedLayer->m_selectedArea);
	}
}

void LayerHandler::updatePhaseFuncData()
{
	qDebug() << m_chartView->width();
	auto maxX = m_chartView->width() - 20;
	auto minX = 20;
	int intervalStart = 256.0 * (float(m_selectedLayer->m_selectedArea.left()-minX) / float(maxX-minX));
	int intervalEnd = 256.0 * (float(m_selectedLayer->m_selectedArea.right()-minX) / float(maxX - minX)) - 1;
	
	QVector<float> textureData;
	textureData.resize((intervalEnd - intervalStart) * 4);

	QColor color = m_selectedLayer->m_layerRGBA;
	for (int i = 0; i < (intervalEnd - intervalStart); i++)
	{
		textureData[i * 4] = float(color.red()) / 255.0;
		textureData[1 + i * 4] = float(color.green()) /255.0;
		textureData[2 + i * 4] = float(color.blue()) / 255.0;
		textureData[3 + i * 4] = float(color.alpha()) / 255.0;
	}

	qDebug() << float(color.red()) / 255.0 << " " << float(color.green()) / 255.0 << " " << float(color.blue()) / 255.0 << " " << float(color.alpha()) / 255.0;
	qDebug() << "Interval " << intervalStart << intervalEnd;
	updatePhaseFunction(intervalStart, intervalEnd, textureData);
}