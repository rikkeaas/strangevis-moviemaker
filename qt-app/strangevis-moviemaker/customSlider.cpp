#include "customSlider.h"

customSlider::customSlider(int minValue, int maxValue, QWidget* parent)
{
	QSlider* horizontalSlider = new QSlider(Qt::Horizontal, this);
	QSpinBox* spinbox = new QSpinBox(this);

	horizontalSlider->setMinimum(minValue);
	horizontalSlider->setMaximum(maxValue);
	spinbox->setMinimum(minValue);
	spinbox->setMaximum(maxValue);

	horizontalSlider->setGeometry(QRect(200, 180, 160, 16));

	connect(spinbox, SIGNAL(valueChanged(int)), horizontalSlider, SLOT(setValue(int)));
	connect(horizontalSlider, SIGNAL(valueChanged(int)), spinbox, SLOT(setValue(int)));

	connect(horizontalSlider, SIGNAL(valueChanged(int)), spinbox, SLOT(setPosition(int)));
}


customSlider::~customSlider()
{

}