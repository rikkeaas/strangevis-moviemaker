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
	m_transferFuncData.resize(256 * 4);
}

void LayerHandler::addLayer(QRect area)
{
	if (area.left() == area.right()) return;
	Layer* newLayer = new Layer(this, area, false, Qt::white);
	newLayer->setStyleSheet("background-color:#6D6D6D; height:45; border-radius:10px;");
	layout()->addWidget(newLayer);
	
	QObject::connect(newLayer, &Layer::clicked, this, &LayerHandler::layerSelected);
	QObject::connect(newLayer, &Layer::updateTransferFunc, this, &LayerHandler::updateTransferFuncData);

	m_layers.append(newLayer);
	layerSelected(newLayer, false);
	updateTransferFuncData();
}

void LayerHandler::layerSelected(Layer* selectedLayer, bool remove)
{
	if (remove)
	{
		deselectSelectedLayer();
		layout()->removeWidget(selectedLayer);
		if (m_selectedLayer == selectedLayer) m_selectedLayer = NULL;
		m_layers.removeAt(m_layers.indexOf(selectedLayer));
		updateLayers();
		delete selectedLayer;
	}
	else
	{
		m_selectedLayer = selectedLayer;
		displayLayer(selectedLayer->m_selectedArea);
	}
}

void LayerHandler::deselectSelectedLayer()
{
	if (m_selectedLayer != NULL)
	{
		undisplayLayer(m_selectedLayer->m_selectedArea);
		m_selectedLayer = NULL;
	}
}

QList<Layer*> LayerHandler::getLayers()
{
	return m_layers;
}

void LayerHandler::setLayers(QList<Layer*> layers)
{

	deselectSelectedLayer();
	while (!layout()->isEmpty()) {
		QLayoutItem* l = layout()->takeAt(0);
		layout()->removeWidget(l->widget());
		delete l->widget();
	}

	m_layers.clear();
	
	foreach(Layer* x, layers) {
		Layer* newLayer = new Layer(this, x->m_selectedArea, true, x->m_layerRGBA);
		newLayer->label->setText(x->label->text());
		newLayer->setStyleSheet("background-color:#6D6D6D; height:45; border-radius:10px;");
		QObject::connect(newLayer, &Layer::clicked, this, &LayerHandler::layerSelected);
		QObject::connect(newLayer, &Layer::updateTransferFunc, this, &LayerHandler::updateTransferFuncData);
		layout()->addWidget(newLayer);

		m_layers.append(newLayer);
	}
}

void LayerHandler::reloadLayers()
{
	Layer* selectedLayerBackup = m_selectedLayer;
	foreach(Layer * x, m_layers) {
		m_selectedLayer = x;
		updateTransferFuncData();
	}
	m_selectedLayer = selectedLayerBackup;
}

void LayerHandler::updateTransferFuncData()
{
	qDebug() << m_chartView->width();
	auto maxX = m_chartView->width() - 20;
	auto minX = 20;
	int intervalStart = 512.0 * (float(m_selectedLayer->m_selectedArea.left()-minX) / float(maxX-minX));
	int intervalEnd = 512.0 * (float(m_selectedLayer->m_selectedArea.right()-minX) / float(maxX - minX)) - 1;
	
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
	updateTransferFunction(intervalStart, intervalEnd, textureData);
}