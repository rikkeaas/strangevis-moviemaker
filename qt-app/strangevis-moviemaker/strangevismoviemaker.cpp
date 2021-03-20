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

    /*
    Toolbox* toolbox = new Toolbox("Cutting Tool", 200, parent);
    auto* anyLayout = new QVBoxLayout();
    anyLayout->addWidget(new QLabel("Some Text in Section", toolbox));
    anyLayout->addWidget(new QPushButton("Button in Section", toolbox));
    toolbox->setContentLayout(*anyLayout);
    */

    // ui.gridLayout->addWidget(toolbox);
    // ui.toolboxMenu->layout()->addWidget(toolbox);

    // Renderer* qtWid = new Renderer();
    QDockWidget* toolbox = new QDockWidget(tr("Toolbox"), this);
    QDockWidget* keyframes = new QDockWidget(tr("Keyframe Handler"), this);
    

    // QSize size = ui.horizontalLayout->widget()->size();
    toolbox->setMinimumSize(QDesktopWidget().availableGeometry(this).size() * 0.15);
    keyframes->setMinimumSize(QDesktopWidget().availableGeometry(this).size() * 0.15);

    toolbox->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbox->setFeatures(QDockWidget::DockWidgetMovable);
    keyframes->setFeatures(QDockWidget::NoDockWidgetFeatures);
    keyframes->setFeatures(QDockWidget::DockWidgetMovable);




    

    toolbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    keyframes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /*

    QWidget* sliders = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    QPushButton* filter1 = new QPushButton(QLatin1String("Filter number 1"));
    QPushButton* filter2 = new QPushButton(QLatin1String("Filter number 2"));
    QPushButton* filter3 = new QPushButton(QLatin1String("Filter number 3"));
    QPushButton* filter4 = new QPushButton(QLatin1String("Filter number 4"));
    QPushButton* filter5 = new QPushButton(QLatin1String("Filter number 5"));

    layout->addWidget(filter1);
    layout->addWidget(filter2);
    layout->addWidget(filter3);
    layout->addWidget(filter4);
    layout->addWidget(filter5);
    sliders->setLayout(layout);
    toolbox->setWidget(sliders);

    */


    auto* cw = ui.centralWidget->layout();

    /*
    // ui.horizontalLayout->addWidget(groupBox);
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(qtWid);
    */

    // setLayout(mainLayout);
    // cw->addWidget(groupBox);
    cw->addWidget(m_renderer);
    this->addDockWidget(Qt::LeftDockWidgetArea, toolbox);
    this->addDockWidget(Qt::LeftDockWidgetArea, keyframes);



    


    
    


    /*
    * 
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


