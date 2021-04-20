#include "strangevismoviemaker.h"
#include "Renderer.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QDesktopWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Renderer* widget = new Renderer(nullptr);
    widget->setMaximumWidth(QDesktopWidget().availableGeometry().width() * 0.8);
    strangevismoviemaker w(widget);
    w.setCentralWidget(widget);
    w.setStyleSheet("background-color: #323232; color: white;");
    w.setWindowTitle("StrangeVis Movie Maker");
    w.showMaximized();
    return a.exec();
}
