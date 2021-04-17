#pragma once
#include <QtCharts>

class HistogramChartView : public QChartView
{
public:
	HistogramChartView(QChart* chart, QWidget* parent);
	//void render(QPainter* painter, const QRectF& target = QRectF(), const QRect& source = QRect(), Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

	void drawForeground(QPainter* painter, const QRectF& rect);

private:
	bool m_drawRubberBand;
	QRubberBand* m_rubberBand;
	QPoint origin;

	QRect m_area;
};