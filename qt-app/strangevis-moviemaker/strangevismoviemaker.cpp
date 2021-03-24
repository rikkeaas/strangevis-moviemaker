#include "strangevismoviemaker.h"
#include "customSlider.h"
#include <QOpenGLWidget>
#include <QFileDialog>
#include "Toolbox.h"
#include <Qlabel>
#include <QPushButton>
#include "Renderer.h"
#include <QDockWIdget>
#include <QtWidgets/qgroupbox.h>
#include <QSize>
#include <QDesktopWidget>
#include <QtCharts>


strangevismoviemaker::strangevismoviemaker(Renderer* renderer, QWidget *parent)
    : QMainWindow(parent)
{
    renderer->setParent(this);
    m_renderer = renderer;
    ui.setupUi(this);
       
    QAction* fileOpenAction = new QAction("Open", this);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    ui.menuFile->addAction(fileOpenAction);

    this->setMinimumSize(1600, 1200);

    QBarSet* barChart = new QBarSet("Density");
    *barChart << 15;
    QBarSeries* series = new QBarSeries();
    series->append(barChart);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Data";
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 15);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto* cw = ui.centralWidget->layout();
    cw->addWidget(m_renderer);

    QDockWidget* toolbox = new QDockWidget(tr("Toolbox"), this);
    QDockWidget* keyframes = new QDockWidget(tr("Keyframe Handler"), this);

    toolbox->setWidget(chartView);
    toolbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbox->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbox->setFeatures(QDockWidget::DockWidgetMovable);
    keyframes->setFeatures(QDockWidget::NoDockWidgetFeatures);
    keyframes->setFeatures(QDockWidget::DockWidgetMovable);

    this->addDockWidget(Qt::LeftDockWidgetArea, toolbox);
    this->addDockWidget(Qt::LeftDockWidgetArea, keyframes);

    

    /*
    QWidget* container = new QWidget(this);
    container->setGeometry(QRect(0, 0, 1000, 1000));
    QVBoxLayout* contLayout = new QVBoxLayout(container);

    customSlider* slider = new customSlider(10, QRect(100, 50, 200, 16), 0, 300, container);
    customSlider* slider2 = new customSlider(2, QRect(100, 100, 300, 16), 0, 10, container);
    customSlider* slider3 = new customSlider(3, QRect(100, 150, 200, 16), 0, 255, container);

    contLayout->addWidget(slider);
    contLayout->addWidget(slider2);
    contLayout->addWidget(slider3);


    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(container);
    
    //layout->addWidget(new QPushButton("Test"));
    setCentralWidget(container);
    //this->setLayout(layout);
    this->setMinimumSize(QSize(1000, 1000));
    this->show();

    //ui.horizontalSlider->setStyleSheet("QSlider::groove:horizontal {color:black; background-color:red;}"
    //                                   "QSlider::handle:horizontal {background-color:blue; height:16px; width: 16px;}");
    //connect(ui.spinBox, SIGNAL(valueChanged(int)), ui.horizontalSlider, SLOT(setValue(int)));
    */
}


void strangevismoviemaker::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Volume File", QString(), "*.dat");

    if (!fileName.isEmpty())
    {
        if (m_renderer->getVolume()->load(fileName))
        {
            qDebug() << "Loaded volume " << fileName;
        }

        else
        {
            qDebug() << "Failed to load volume " << fileName;
        }
    }

}


