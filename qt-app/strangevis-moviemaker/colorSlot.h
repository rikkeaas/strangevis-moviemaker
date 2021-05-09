#pragma once
#include <QWidget>

class ColorSlot : public QWidget
{
	Q_OBJECT

public:
	ColorSlot(int size, QColor inColor);
	void publicMousePress();
	QColor color;
signals:
	void colorChange(QColor color);
public slots:
	void setAnimationIsPlaying(bool);
protected:
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
private:
	bool animationIsPlaying = false;
};