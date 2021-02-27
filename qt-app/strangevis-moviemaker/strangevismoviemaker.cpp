#include "strangevismoviemaker.h"
#include "customSlider.h"

strangevismoviemaker::strangevismoviemaker(QWidget *parent)
    : QMainWindow(parent)
{
    //ui.setupUi(this);

    customSlider* slider = new customSlider(0, 255, this);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(slider);
    layout->addWidget(new QPushButton("Test"));
    setCentralWidget(slider);
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