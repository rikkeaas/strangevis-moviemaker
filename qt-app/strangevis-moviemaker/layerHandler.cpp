#include "layerHandler.h"
#include <QDebug>
#include <QVBoxLayout>

LayerHandler::LayerHandler() : QWidget()
{
	setMaximumHeight(400);
	setMinimumHeight(400);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setAlignment(Qt::AlignTop);
	
	setLayout(layout);
	m_selectedLayer = NULL;
	
	// Stygg hardkoding.... fiks dette!!
	minX = 20;
	maxX = 358;
	m_phaseFuncData.resize(256 * 4);
}

void LayerHandler::addLayer(QRect area)
{
	Layer* newLayer = new Layer(this, area);
	newLayer->setMaximumHeight(30);
	newLayer->setMinimumHeight(30);
	newLayer->setStyleSheet("background-color:#6D6D6D; height:30;");
	layout()->addWidget(newLayer);
	
	QObject::connect(newLayer, &Layer::clicked, this, &LayerHandler::layerSelected);

	m_layers.append(newLayer);
}

void LayerHandler::redChanged(int newRed)
{
	if (m_selectedLayer != NULL)
	{
		m_selectedLayer->red = newRed;
		updatePhaseFuncData();
	}
}

void LayerHandler::greenChanged(int newGreen)
{
	if (m_selectedLayer != NULL)
	{
		m_selectedLayer->green = newGreen;
		updatePhaseFuncData();
	}
}

void LayerHandler::blueChanged(int newBlue)
{
	if (m_selectedLayer != NULL)
	{
		m_selectedLayer->blue = newBlue;
		updatePhaseFuncData();
	}
}

void LayerHandler::alphaChanged(int newAlpha)
{
	if (m_selectedLayer != NULL)
	{
		m_selectedLayer->alpha = newAlpha;
		updatePhaseFuncData();
	}
}


void LayerHandler::layerSelected(Layer* selectedLayer, bool remove)
{
	if (remove)
	{
		undisplayLayer(selectedLayer->m_selectedArea);
		layout()->removeWidget(selectedLayer);
		delete selectedLayer;

	}
	else
	{
		qDebug() << selectedLayer->m_selectedArea.left() << selectedLayer->m_selectedArea.right();
		m_selectedLayer = selectedLayer;
		displayLayer(selectedLayer->m_selectedArea);
		updateRedSlider(selectedLayer->red);
		updateGreenSlider(selectedLayer->green);
		updateBlueSlider(selectedLayer->blue);
		updateAlphaSlider(selectedLayer->alpha);
	}
}

void LayerHandler::updatePhaseFuncData()
{
	int intervalStart = 256.0 * (float(m_selectedLayer->m_selectedArea.left()-minX) / float(maxX-minX));
	int intervalEnd = 256.0 * (float(m_selectedLayer->m_selectedArea.right()-minX) / float(maxX - minX)) - 1;
	
	QVector<float> textureData;
	textureData.resize((intervalEnd - intervalStart) * 4);

	for (int i = 0; i < (intervalEnd - intervalStart); i++)
	{
		textureData[i * 4] = float(m_selectedLayer->red) / 300.0;
		textureData[1 + i * 4] = float(m_selectedLayer->green) /300.0;
		textureData[2 + i * 4] = float(m_selectedLayer->blue) / 300.0;
		textureData[3 + i * 4] = float(m_selectedLayer->alpha) / 300.0;
	}

	qDebug() << m_selectedLayer->red << m_selectedLayer->green << m_selectedLayer->blue << m_selectedLayer->alpha;
	qDebug() << intervalStart << intervalEnd;
	updatePhaseFunction(intervalStart, intervalEnd, textureData);
}