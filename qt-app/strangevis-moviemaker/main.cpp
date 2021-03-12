#include "strangevismoviemaker.h"
#include "Renderer.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    strangevismoviemaker w;
    w.setStyleSheet("background-color: #323232; color: white;");
    w.setWindowTitle("StrangeVis Movie Maker");
    w.showMaximized();
    return a.exec();
}
