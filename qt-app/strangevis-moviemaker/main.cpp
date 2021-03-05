#include "strangevismoviemaker.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    strangevismoviemaker w;
    w.setMinimumWidth(800);
    w.setMinimumHeight(600);
    w.setStyleSheet("background-color: #323232; color: white;");
    w.setWindowTitle("StrangeVis Movie Maker");
    w.show();

    return a.exec();
}
