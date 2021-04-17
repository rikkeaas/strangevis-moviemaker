#include "histogramChartView.h"

HistogramChartView::HistogramChartView(QChart* chart, QWidget* parent) : QChartView(chart, parent)
{
	setRubberBand(QChartView::RectangleRubberBand);
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void HistogramChartView::mousePressEvent(QMouseEvent* event)
{
    origin = event->pos();
    m_rubberBand->setGeometry(QRect(origin, QSize()));
    m_rubberBand->show();

}

void HistogramChartView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_rubberBand->isVisible())
    {
        m_rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    }
}

void HistogramChartView::mouseReleaseEvent(QMouseEvent* event)
{
    
    m_rubberBand->hide();
    QRect rect = m_rubberBand->geometry();
    rect.setBottom(height());
    
    m_area = rect;

    qDebug() << "do nothing";
}

void HistogramChartView::drawForeground(QPainter* painter, const QRectF& rect)
{
    qDebug() << "rendering!!";
    if (!m_area.isNull())
    {
        qDebug() << "painting";
        painter->setBrush(Qt::Dense7Pattern);
        painter->setPen(QColor(255, 0, 0, 127));

        painter->drawRect(m_area);
    }

}