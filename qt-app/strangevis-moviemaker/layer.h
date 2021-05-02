#pragma once
#include <QWidget>
#include <QLineEdit>


class Layer : public QWidget
{
	Q_OBJECT
public:
	Layer(QWidget* parent, QRect area, bool isExisting, QColor color);
	QRect m_selectedArea;
	QColor m_layerRGBA;
	QLineEdit* label;

signals:
	void clicked(Layer* selectedLayer, bool remove);
	void updatePhaseFunc();
	void labelUpdated();
public slots:
	void colorChange(QColor color);
protected:
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};