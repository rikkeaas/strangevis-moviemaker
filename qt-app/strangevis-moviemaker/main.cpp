#include "strangevismoviemaker.h"
#include "Renderer.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Renderer* widget = new Renderer();

    QMainWindow w;
    w.setCentralWidget(widget);
    w.resize(QSize(800, 600));
    w.show();
    return a.exec();

    /*QApplication a(argc, argv);
    strangevismoviemaker w;
    w.setStyleSheet("background-color: #323232; color: white;");
    w.setWindowTitle("StrangeVis Movie Maker");
    w.show();
    return a.exec();*/
}
