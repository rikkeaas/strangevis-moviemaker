#include "histogram.h"
#include "histogramChartView.h"
#include "model.h"
#include "customSlider.h"
#include <cfloat>
#include <math.h>
using namespace std;


void Histogram::updateHistogramYScaling(bool displayLog, int clamp)
{
    chartViewP->chart()->removeAllSeries();
    auto values = m_renderer->getVolume()->getDataset();
    QBarSet* barChart = new QBarSet("Density");
    QStringList categories;
    
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
            if (displayLog) *barChart << log10(val);
            else if (clamp > 0) *barChart << qMin(clamp, val);
            else *barChart << val;
            QString str = "d";
            str.append(i);
            categories << str;
        }
        qDebug() << "Vals: " << (s - skipped) << "Skipped: " << skipped;
    }

    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);
    chartViewP->chart()->addSeries(series);

}

Histogram::Histogram(Renderer* renderer, bool displayLog, int clamp) : QWidget() {
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
            if (displayLog) *barChart << log10(val);
            else if (clamp > 0) *barChart << qMin(clamp, val);
            else *barChart << val;
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

    chartViewP = chartView;

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

   
void Histogram::updateTransferFunction(int start, int end, QVector<float> textureData)
{
    m_renderer->getTransferFunction()->updateTransferFunction(start, end, &textureData);
    m_renderer->setLayers(m_layerHandler->getLayers());
}

void Histogram::focusOutEvent(QFocusEvent* event)
{
    m_layerHandler->deselectSelectedLayer();
    event->accept();
}