#include "layer.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QColorDialog>
#include <QPushButton>

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

	QColor color = QColorDialog::getColor(Qt::white, nullptr, "Layer color", { QColorDialog::DontUseNativeDialog, QColorDialog::ShowAlphaChannel });
	m_layerRGBA = color;

	QWidget* colorSample = new QWidget();
	colorSample->setStyleSheet("QWidget {background-color: " + color.name() + "; } QWidget:hover {border: 1px solid black;}");
	colorSample->setAutoFillBackground(true);
	colorSample->setFixedWidth(height());
	colorSample->setFixedHeight(height());
	colorSample->update();
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


// some hacky code from Stack Overflow
// https://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
void Layer::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}