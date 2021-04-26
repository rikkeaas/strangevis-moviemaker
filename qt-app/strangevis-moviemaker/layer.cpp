#include "layer.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QColorDialog>
#include <QPushButton>
#include "colorSlot.h"

Layer::Layer(QWidget* parent, QRect area) : QWidget(parent)
{
	m_selectedArea = area;
	QLineEdit* label = new QLineEdit();
	QHBoxLayout* layout = new QHBoxLayout();
	label->setText("Layer name...");
	int id = QFontDatabase::addApplicationFont("fonts/Roboto-Regular.ttf");
	QString robotoHeader = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont f(robotoHeader, 10);
	label->setFont(f);
	label->setTextMargins(5, 5, 5, 5);
	label->setStyleSheet("QLineEdit {background-color: #4C4C4C;}");
	layout->addWidget(label);
	

	//layout->setMargin(5);
	layout->setContentsMargins(10, 10, 10, 10);

	ColorSlot* colorSample = new ColorSlot(height());
	QObject::connect(colorSample, &ColorSlot::colorChange, this, &Layer::colorChange);
	colorSample->publicMousePress();
	layout->addWidget(colorSample);
	
	setLayout(layout);
}

void Layer::mouseDoubleClickEvent(QMouseEvent* event)
{
	clicked(this, true);
}

void Layer::mousePressEvent(QMouseEvent* event)
{
	clicked(this, false);
}

void Layer::colorChange(QColor color)
{
	m_layerRGBA = color;
	clicked(this, false);
	updatePhaseFunc();
}

// some hacky code from Stack Overflow
// https://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
void Layer::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

