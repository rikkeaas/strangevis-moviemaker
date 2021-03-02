#include "strangevismoviemaker.h"
#include "customSlider.h"

strangevismoviemaker::strangevismoviemaker(QWidget *parent)
    : QMainWindow(parent)
{
    //ui.setupUi(this);

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
}

void strangevismoviemaker::buttonClicked()
{
    if (ui.pushButton->text() == "DOnt click me")
    {
        ui.pushButton->setText("Click me");
    }
    else {
        ui.pushButton->setText("DOnt click me");
    }
}