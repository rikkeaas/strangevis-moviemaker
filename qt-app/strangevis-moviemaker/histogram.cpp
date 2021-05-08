#include "histogram.h"
#include "model.h"
#include "customSlider.h"
#include <cfloat>
#include <math.h>
using namespace std;

Histogram::Histogram(Renderer* renderer, bool displayLog, int clamp) : QWidget()
{
    m_renderer = renderer;
    setFocusPolicy(Qt::StrongFocus);

    QBarSet* barChart = computeHistogram(displayLog, clamp);

    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);

    QChart* chart = new QChart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->legend()->setVisible(false);
    chart->setBackgroundBrush(QColor(109, 109, 109));
    chart->setPlotAreaBackgroundVisible(false);
    chart->setBackgroundRoundness(15);
    chart->setMaximumHeight(QDesktopWidget().availableGeometry().height() * 0.20);
    chart->setContentsMargins(0, 0, 0, 0);
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    chartView = new HistogramChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->setAcceptHoverEvents(true);
    chartView->setContentsMargins(0, 0, 0, 0);

    QMargins margins = chart->margins();

    QVBoxLayout* histoLayout = new QVBoxLayout();
    setLayout(histoLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(getHistogram());

    m_layerHandler = new LayerHandler(chartView);
    layout()->addWidget(m_layerHandler);

    QObject::connect(m_layerHandler, &LayerHandler::displayLayer, chartView, &HistogramChartView::showLayerSelection);
    QObject::connect(m_layerHandler, &LayerHandler::undisplayLayer, chartView, &HistogramChartView::unshowLayerSelection);

    QObject::connect(chartView, &HistogramChartView::addLayer, m_layerHandler, &LayerHandler::addLayer);

    QObject::connect(m_layerHandler, &LayerHandler::updateLayers, this, &Histogram::updateLayers);
    QObject::connect(m_layerHandler, &LayerHandler::updateTransferFunction, this, &Histogram::updateTransferFunction);

}


void Histogram::updateHistogramYScaling(bool displayLog, int clamp)
{
    QBarSet* barChart = computeHistogram(displayLog, clamp);
    chartView->chart()->removeAllSeries();
    QBarSeries* series = new QBarSeries();
    series->append(barChart);
    series->setBarWidth(1);
    chartView->chart()->addSeries(series);
}

QBarSet* Histogram::computeHistogram(bool log, int clamp)
{
    auto values = m_renderer->getVolume()->getDataset();
    QBarSet* barChart = new QBarSet("Density");
    QStringList categories;

    if (!values.isEmpty()) {

        int showValuesAbove = 0;
        int skipStep = 10;
        int roundTo = 40;

        std::map<float, int> bin = binData(values, skipStep, roundTo);
        int s = bin.size();

        int skipped = 0;
        std::map<float, int>::iterator itr;
        itr = bin.begin();


        for (int i = 0; i < s; i++) {
            float key = itr->first;
            int val = bin.at(key);
            ++itr;
            if (val < showValuesAbove) {
                skipped += 1;
                continue;
            }
            if (log) *barChart << log10(val);
            else if (clamp > 0) *barChart << qMin(clamp, val);
            else *barChart << val;
            QString str = "d";
            str.append(i);
            categories << str;
        }
        qDebug() << "Vals: " << (s - skipped) << "Skipped: " << skipped;
    }

    return barChart;
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
    return chartView;
}

   
void Histogram::updateTransferFunction(int start, int end, QVector<float> textureData)
{
    m_renderer->getTransferFunction()->updateTransferFunction(start, end, &textureData);
    m_renderer->setLayers(m_layerHandler->getLayers());
}

void Histogram::updateLayers()
{
    m_renderer->setLayers(m_layerHandler->getLayers());
}

void Histogram::focusOutEvent(QFocusEvent* event)
{
    m_layerHandler->deselectSelectedLayer();
    event->accept();
}