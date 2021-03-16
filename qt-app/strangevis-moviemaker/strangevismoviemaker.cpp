#include "strangevismoviemaker.h"
#include "customSlider.h"
#include <QOpenGLWidget>
#include <QFileDialog>
#include "Toolbox.h"
#include <Qlabel>
#include <QPushButton>


strangevismoviemaker::strangevismoviemaker(Renderer* renderer, QWidget *parent)
    : QMainWindow(parent)
{
    renderer->setParent(this);
    m_renderer = renderer;
    ui.setupUi(this);
       
    QAction* fileOpenAction = new QAction("Open", this);
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    ui.menuFile->addAction(fileOpenAction);

    Toolbox* toolbox = new Toolbox("Cutting Tool", 200, parent);
    auto* anyLayout = new QVBoxLayout();
    anyLayout->addWidget(new QLabel("Some Text in Section", toolbox));
    anyLayout->addWidget(new QPushButton("Button in Section", toolbox));
    toolbox->setContentLayout(*anyLayout);

    // ui.gridLayout->addWidget(toolbox);
    ui.toolboxMenu->layout()->addWidget(toolbox);


    
    


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


