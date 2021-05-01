#include "colorSlot.h"
#include <QColorDIalog>
#include <QStyleOption>
#include <QPainter>

ColorSlot::ColorSlot(int size, QColor inColor) : QWidget()
{
	setAutoFillBackground(true);
	setFixedWidth(size);
	setFixedHeight(size);
	color = inColor;
	setStyleSheet("QWidget {background-color: " + color.name() + "; } QWidget:hover {border: 1px solid black;}");
}


void ColorSlot::mousePressEvent(QMouseEvent* event)
{
	QColor newCol = QColorDialog::getColor(color, nullptr, "Layer color", { QColorDialog::DontUseNativeDialog, QColorDialog::ShowAlphaChannel });
	if (newCol.isValid())
	{
		color = newCol;
		setStyleSheet("QWidget {background-color: " + color.name() + "; } QWidget:hover {border: 1px solid black;}");
		colorChange(color);
		update();
	}
}

void ColorSlot::publicMousePress()
{
	mousePressEvent(NULL);
}

void ColorSlot::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}