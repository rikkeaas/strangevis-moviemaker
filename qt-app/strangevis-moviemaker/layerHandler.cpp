#include "layerHandler.h"
#include <QDebug>
#include <QVBoxLayout>

LayerHandler::LayerHandler() : QWidget()
{
	setMaximumHeight(400);
	setMinimumHeight(400);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin(0);
	setLayout(layout);
	m_selectedLayer = NULL;
}

void LayerHandler::addLayer(QRect area)
{
	Layer* test = new Layer(this, area);
	test->setObjectName("test");
	test->setStyleSheet("background-color:#6D6D6D;");
	layout()->addWidget(test);
	qDebug() << "add layer";
	QObject::connect(test, &Layer::clicked, this, &LayerHandler::layerSelected);
}

void LayerHandler::redChanged(int newRed)
{
	if (m_selectedLayer != NULL)
		m_selectedLayer->red = newRed;
}

void LayerHandler::greenChanged(int newGreen)
{
	if (m_selectedLayer != NULL)
		m_selectedLayer->green = newGreen;
}

void LayerHandler::blueChanged(int newBlue)
{
	if (m_selectedLayer != NULL)
		m_selectedLayer->blue = newBlue;
}

void LayerHandler::alphaChanged(int newAlpha)
{
	if (m_selectedLayer != NULL)
		m_selectedLayer->alpha = newAlpha;
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
		m_selectedLayer = selectedLayer;
		displayLayer(selectedLayer->m_selectedArea);
		updateRedSlider(selectedLayer->red);
		updateGreenSlider(selectedLayer->green);
		updateBlueSlider(selectedLayer->blue);
		updateAlphaSlider(selectedLayer->alpha);
	}
}