#pragma once
#include <QWidget>

class Layer : public QWidget
{
	Q_OBJECT
public:
	Layer(QWidget* parent, QRect area);
	QRect m_selectedArea;
	int red = 300;
	int green = 0;
	int blue = 0;
	int alpha = 300;

signals:
	void clicked(Layer* selectedLayer, bool remove);
protected:
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};