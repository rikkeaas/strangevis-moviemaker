#include <QColor>
#include <QVBoxLayout>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "histogram.h"

namespace {
	const QColor MATERIAL_GRAY{ 0x9E, 0x9E, 0x9E };
}


Histogram::Histogram(QWidget* parent)
	: QWidget(parent), chart{} {
	auto column = new QVBoxLayout{ this };

	auto chart_view = new QtCharts::QChartView{ this };
	chart = chart_view;
	chart_view->setChart(new QtCharts::QChart);
	chart_view->setRenderHint(QPainter::Antialiasing);
	chart_view->setToolTip("Histogram");
	column->addWidget(chart_view);
	
	setLayout(column);
}

void Histogram::plot(const histogram_t& histogram)
{
	// histogram (upper line)
	auto series{ new QtCharts::QLineSeries{this} };

	// bottom straight line
	auto bottom_series{ new QtCharts::QLineSeries{this} };

	// populate series
	Q_ASSERT(histogram.size() == 256);
	for (auto i{ 0 }; i < 256; ++i) {
		series->append(i, histogram[i]);
		bottom_series->append(i, 0);
	}

	// merge line series into area
	auto area_series{ new QtCharts::QAreaSeries{series, bottom_series} };

	// plot
	const auto widget{ this->chart };
	widget->chart()->legend()->hide();
	widget->chart()->removeAllSeries();
	widget->chart()->addSeries(area_series);

	area_series->setColor(MATERIAL_GRAY);
	widget->chart()->setTitle("Histogram");
}
