#pragma once
#include <QWidget>

class Layer : public QWidget
{
	Q_OBJECT
public:
	Layer(QWidget* parent, QRect area);
	QRect m_selectedArea;
	QColor m_layerRGBA;

signals:
	void clicked(Layer* selectedLayer, bool remove);
	void updatePhaseFunc();
public slots:
	void colorChange(QColor color);
protected:
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};