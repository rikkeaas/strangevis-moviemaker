#include "layer.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>

Layer::Layer(QWidget* parent, QRect area) : QWidget(parent)
{
	m_selectedArea = area;
	QLineEdit* label = new QLineEdit(this);
	QHBoxLayout* layout = new QHBoxLayout();
	label->setText("Layer name...");
	layout->addWidget(label);
}

void Layer::mouseDoubleClickEvent(QMouseEvent* event)
{
	clicked(this, true);
}

void Layer::mousePressEvent(QMouseEvent* event)
{
	clicked(this, false);
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