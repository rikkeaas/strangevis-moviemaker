#include "histogram.h"
#include "histogramChartView.h"
#include "model.h"
#include "customSlider.h"
#include <cfloat>
using namespace std;


Histogram::Histogram(Renderer* renderer) : QWidget() {
    m_renderer = renderer;
    auto values = renderer->getVolume()->getDataset();
    QBarSet* barChart = new QBarSet("Density");
    barChart->setBorderColor(Qt::white);
    barChart->setColor(Qt::white);
    QStringList categories;
   
    setFocusPolicy(Qt::StrongFocus);

    if (!values.isEmpty()) {
       
        int showValuesAbove = 0;
        int skipStep = 10;
        int roundTo = 40;

        std::map<float, int> bin = binData(values, skipStep, roundTo);
        int s = bin.size();
        // qDebug() << "Map size: " << s;
        // qDebug() << "Map content: " << bin;

        int skipped = 0;
        std::map<float, int>::iterator itr;
        itr = bin.begin();


        for (int i = 0; i < s; i++) {
            float key = itr->first;
            // qDebug() << "Key: " << key << " Value: " << bin.at(key);
            int val = bin.at(key);
            ++itr;
            if (val < showValuesAbove) {
                skipped += 1;
                continue;
            }
            *barChart << val;
            QString str = "d";
            str.append(i);
            categories << str;
        }
        qDebug() << "Vals: " << (s-skipped) << "Skipped: " << skipped;
    }

    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);

    QChart* chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->legend()->setVisible(false);
    // chart->setBackgroundVisible(false);
    chart->setBackgroundBrush(QColor(109,109,109));
    chart->setPlotAreaBackgroundVisible(false);
    chart->setBackgroundRoundness(15);
    chart->setMaximumHeight(QDesktopWidget().availableGeometry().height() * 0.20);
    // chart->setMargins({ 0, 0, 0, 0 });
    chart->setContentsMargins(0, 0, 0, 0);
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    HistogramChartView* chartView = new HistogramChartView(chart, this);
    //chartView->setChart(chart);
    
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->setAcceptHoverEvents(true);
    chartView->setContentsMargins(0, 0, 0, 0);
  
    QMargins margins = chart->margins();
    
    qDebug() << margins;
    //qDebug() << "Width of chart " << chart-> << " bars at " << series->barWidth() << " times nb of bars: " << barChart->count();

    qDebug() << chartView->width();

    chartViewP = chartView;
    //chartViewP->setRubberBand(QChartView::RectangleRubberBand);
    //qDebug() << barChart-> << barChart->at(1) << barChart->at(2) << barChart->at(3);
    //QObject::connect(barChart, &QBarSet::hovered, this, &Histogram::showHovering);
    //QObject::connect(barChart, &QBarSet::clicked, this, &Histogram::registerClick);
    //QObject::connect(barChart, &QBarSet::released, this, &Histogram::endClick);

    QVBoxLayout* histoLayout = new QVBoxLayout();
    setLayout(histoLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(getHistogram());
 
    m_layerHandler = new LayerHandler(chartViewP);
    layout()->addWidget(m_layerHandler);

    
    QObject::connect(m_layerHandler, &LayerHandler::displayLayer, chartViewP, &HistogramChartView::showLayerSelection);
    QObject::connect(m_layerHandler, &LayerHandler::undisplayLayer, chartViewP, &HistogramChartView::unshowLayerSelection);

    QObject::connect(chartViewP, &HistogramChartView::addLayer, m_layerHandler, &LayerHandler::addLayer);

    QObject::connect(m_layerHandler, &LayerHandler::updateTransferFunction, this, &Histogram::updateTransferFunction);

}

std::map<float, int> Histogram::binData(QVector<unsigned short> values, int skipStep, int roundTo) {
    std::map<float, int> occurences;
    for (int i = 0; i*4 < values.size(); i += skipStep) {
        float val = roundNearest(roundTo, values.at(i*4));
        if (occurences[val] > 1500)
        {
            continue;
        }
        if (occurences.count(val) > 0) {
            occurences[val] += 1;
        }
        else {
            occurences.insert(make_pair(val, 1));
        }
    }
    return occurences;
}

float Histogram::roundNearest(int roundTo, float d)
{
    int r = roundTo;
    int d_i = d;
    return ((d_i % r) < (r/2)) ? d_i - (d_i % r) : d_i + (r - (d_i % r));
}

QChartView* Histogram::getHistogram()
{
    return chartViewP;
}

/*
void Histogram::showHovering(bool status, int index) 
{

    QPoint p = chartViewP->mapFromGlobal(QCursor::pos());// chartView->mapFromGlobal(QCursor::pos());
    QGraphicsItem* bar = chartViewP->itemAt(p);
   // qDebug() << bar;
  
    QGraphicsRectItem* rect = qgraphicsitem_cast<QGraphicsRectItem*>(chartViewP->itemAt(p));
    if (rect == NULL) {
        //qDebug() << "no rect";
        return;
    }

    if (status) {
        //qDebug() << "hovering " << index;
        hoverItem.setParentItem(bar);
        hoverItem.setRect(bar->boundingRect());
        hoverItem.show();
    }
    else {
        //qDebug() << "not hovering " << index;
        hoverItem.setParentItem(nullptr);
        hoverItem.hide();
    }
}
*/

/*
void Histogram::registerClick(int index)
{
    qDebug() << "Histogram click";
    QPoint p = chartViewP->mapFromGlobal(QCursor::pos());// chartView->mapFromGlobal(QCursor::pos());
    QGraphicsItem* bar = chartViewP->itemAt(p);
    //qDebug() << bar;
    QGraphicsRectItem* rect = qgraphicsitem_cast<QGraphicsRectItem*>(chartViewP->itemAt(p));
    if (rect == NULL) {
        //qDebug() << "no rect";
        return;
    }

    if (endClick)
    {
        endClick = false;
        interval.append(index);
        qDebug() << interval.at(interval.size() - 2) << " - " << interval.at(interval.size() - 1);
        int size = chartViewP->items().size();
        int start = 256.0 * float(interval.at(interval.size() - 2)) / float(size);
        int end = 256.0 * float(interval.at(interval.size() - 1)) / float(size);
        if (end < start)
        {
            auto temp = start;
            start = end;
            end = temp;
        }
        QVector<float> textureData;
        textureData.resize((end-start) * 4);

        for (int i = 0; i < (end - start) * 4; i++) 
        {
            textureData[i] = 0.0;
        }

        m_renderer->getTransferFunction()->updateTransferFunction(start, end, &textureData);
    }
    else
    {
        endClick = true;
    }

    clickItem.setParentItem(bar);
    clickItem.setRect(bar->boundingRect());
    clickItem.show();
    
    interval.append(index);
}
*/

void Histogram::updateTransferFunction(int start, int end, QVector<float> textureData)
{
    m_renderer->getTransferFunction()->updateTransferFunction(start, end, &textureData);
    m_renderer->setLayers(m_layerHandler->getLayers());
}

void Histogram::focusOutEvent(QFocusEvent* event)
{
    m_layerHandler->deselectSelectedLayer();
    event->accept();
    qDebug() << "focus out?";
}