#include "customSlider.h"

#include <QFile>
#include <iostream>


CustomSlider::CustomSlider(int color, QRect sliderPos, int minValue, int maxValue, QWidget* parent)
{
	m_horizontalSlider = new QSlider(Qt::Horizontal, this);
	m_spinbox = new QSpinBox(this);

	m_horizontalSlider->setMinimum(minValue);
	m_horizontalSlider->setMaximum(maxValue);
	m_spinbox->setMinimum(minValue);
	m_spinbox->setMaximum(maxValue);

	m_horizontalSlider->setGeometry(sliderPos);
	m_spinbox->setGeometry(QRect(sliderPos.x(), sliderPos.y() - 40, 55, 30));

	//red : #FF9781 #FF6A4B #FF2D00
	//green : #9EFFA6 #5AFF68 #00DE13
	// blue: 66e bbf 55f
	
	if (color < 1 || color > 3) color = 1; // If not valid color, slider will be red

	QFile cf;
	switch (color)
	{
		case 1: // red
			cf.setFileName("./StyleSheets/red_slider.qss");
			break;
		case 2: // blue
			cf.setFileName("./StyleSheets/blue_slider.qss");
			break;
		case 3: // green
			cf.setFileName("./StyleSheets/green_slider.qss");
			break;
	}
	cf.open(QFile::ReadOnly);

	QFile f("./StyleSheets/general_slider.qss");
	f.open(QFile::ReadOnly);
	m_horizontalSlider->setStyleSheet(f.readAll() + cf.readAll());

	// Setting up links between spinbox and slider
	connect(m_spinbox, SIGNAL(valueChanged(int)), m_horizontalSlider, SLOT(setValue(int)));
	connect(m_horizontalSlider, SIGNAL(valueChanged(int)), m_spinbox, SLOT(setValue(int)));

	connect(m_horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));

	connect(m_horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(sendValueSignal(int)));
	
}

void CustomSlider::sendValueSignal(int val)
{
	valueChanged(val);
}

void CustomSlider::setPosition(int newVal)
{
	QRect sliderGeo = m_horizontalSlider->geometry();
	QRect spinboxGeo = m_spinbox->geometry();

	int width = spinboxGeo.width() * (float)newVal / (float)m_horizontalSlider->maximum();
	int xpos = sliderGeo.x() + (((float) sliderGeo.width() / (float)(m_horizontalSlider->maximum())) * newVal) - width;

	m_spinbox->setGeometry(QRect(xpos, spinboxGeo.y(), spinboxGeo.width(), spinboxGeo.height()));
}

void CustomSlider::setValue(int val)
{
	m_horizontalSlider->setValue(val);
}

CustomSlider::~CustomSlider()
{

}

