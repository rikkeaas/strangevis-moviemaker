#include "customSlider.h"

#include <QFile>
#include <iostream>

customSlider::customSlider(int color, QRect sliderPos, int minValue, int maxValue, QWidget* parent)
{
	m_horizontalSlider = new QSlider(Qt::Horizontal, this);
	m_spinbox = new QSpinBox(this);

	m_horizontalSlider->setMinimum(minValue);
	m_horizontalSlider->setMaximum(maxValue);
	m_spinbox->setMinimum(minValue);
	m_spinbox->setMaximum(maxValue);

	m_horizontalSlider->setGeometry(sliderPos);
	m_spinbox->setGeometry(QRect(sliderPos.x(), sliderPos.y() - 40, 50, 30));

	//red : #FF9781 #FF6A4B #FF2D00
	//green : #9EFFA6 #5AFF68 #00DE13
	// blue: 66e bbf 55f
	QFile f("./StyleSheets/sliderStyle.qss");
	f.open(QFile::ReadOnly);
	m_horizontalSlider->setStyleSheet(f.readAll());

	QString colorStyle;
	switch (color)
	{
		case 1: // red
			colorStyle = "QSlider::handle:horizontal{background-color: red}";//"QSlider::sub-page:horizontal{background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #FF9781, stop: 1 #FF6A4B);background: qlineargradient(x1 : 0, y1 : 0.2, x2 : 1, y2 : 1, stop : 0 #FF6A4B, stop: 1 #FF2D00);}";
	}

	m_horizontalSlider->setStyleSheet(colorStyle);

	connect(m_spinbox, SIGNAL(valueChanged(int)), m_horizontalSlider, SLOT(setValue(int)));
	connect(m_horizontalSlider, SIGNAL(valueChanged(int)), m_spinbox, SLOT(setValue(int)));

	connect(m_horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));


}

void customSlider::setPosition(int newVal)
{
	QRect sliderGeo = m_horizontalSlider->geometry();
	QRect spinboxGeo = m_spinbox->geometry();

	int width = spinboxGeo.width() * (float)newVal / (float)m_horizontalSlider->maximum();
	int xpos = sliderGeo.x() + (((float) sliderGeo.width() / (float)(m_horizontalSlider->maximum())) * newVal) - width;

	m_spinbox->setGeometry(QRect(xpos, spinboxGeo.y(), spinboxGeo.width(), spinboxGeo.height()));
}


customSlider::~customSlider()
{

}
