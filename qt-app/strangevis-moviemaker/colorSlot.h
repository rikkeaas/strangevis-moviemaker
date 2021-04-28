#pragma once
#include <QWidget>

class ColorSlot : public QWidget
{
	Q_OBJECT

public:
	ColorSlot(int size);
	void publicMousePress();
	QColor color;
signals:
	void colorChange(QColor color);
protected:
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};