#include "histogramChartView.h"


HistogramChartView::HistogramChartView(QChart* chart, QWidget* parent) : QChartView(chart, parent)
{
	setRubberBand(QChartView::RectangleRubberBand);
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    //hoverItem.setBrush(QBrush(Qt::red));
    //hoverItem.setPen(Qt::NoPen);
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


    QChartView::mouseMoveEvent(event);
}


void HistogramChartView::mouseReleaseEvent(QMouseEvent* event)
{
    //QPoint p = mapFromGlobal(QCursor::pos());// chartView->mapFromGlobal(QCursor::pos());
    //QGraphicsItem* bar = itemAt(p);
    
   

    m_rubberBand->hide();
    QRect rect = m_rubberBand->geometry();
    rect.setLeft(qMax(20, rect.left()));
    rect.setRight(qMin(width() - 20, rect.right()));
    rect.setBottom(height()-1-20);
    rect.setTop(0+20);

    m_area = rect;

    addLayer(m_area);

    update();
    
}

void HistogramChartView::leaveEvent(QEvent* event)
{
    QRect none;
    m_area = none;
    update();
    
}

void HistogramChartView::drawForeground(QPainter* painter, const QRectF& rect)
{
    if (!m_area.isNull())
    {
        painter->setBrush(Qt::Dense6Pattern);
        painter->setPen(QColor(255, 0, 0, 127));

        painter->drawRect(m_area);
    }
    if (!m_selectedLayer.isNull())
    {
        painter->setBrush(Qt::Dense4Pattern);
        painter->setPen(QColor(20, 20, 20, 127));

        painter->drawRect(m_selectedLayer);
    }
}


QRect HistogramChartView::getSelectedArea()
{
    return m_area;
}

void HistogramChartView::showLayerSelection(QRect layerSelectionArea)
{
    m_selectedLayer = layerSelectionArea;
    update();
}

void HistogramChartView::unshowLayerSelection(QRect layerSelection)
{
    if (m_selectedLayer == layerSelection)
    {
        QRect none;
        m_selectedLayer = none;
        update();
    }
}