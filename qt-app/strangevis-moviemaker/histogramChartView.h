#pragma once
#include <QtCharts>


class HistogramChartView : public QChartView
{
	Q_OBJECT
public:
	HistogramChartView(QChart* chart, QWidget* parent);
	QRect getSelectedArea();

public slots:
	void showLayerSelection(QRect layerSelectionArea);
	void unshowLayerSelection(QRect layerSelection);
	void setAnimationIsPlaying(bool);
signals:
	void addLayer(QRect area);

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void leaveEvent(QEvent* event);
	void drawForeground(QPainter* painter, const QRectF& rect);

private:
	QRubberBand* m_rubberBand;
	QPoint origin;
	QRect m_area;
	QRect m_selectedLayer;
	bool showSelection = false;
	bool animationIsPlaying = false;
};