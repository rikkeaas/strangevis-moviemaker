#include "strangevismoviemaker.h"
#include "Renderer.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Renderer* widget = new Renderer();
    strangevismoviemaker w;
    w.setCentralWidget(widget);
    w.setStyleSheet("background-color: #323232; color: white;");
    w.setWindowTitle("StrangeVis Movie Maker");
    w.showMaximized();
    return a.exec();
}
