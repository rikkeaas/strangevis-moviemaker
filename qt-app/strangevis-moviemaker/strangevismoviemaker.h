#pragma once

#include <QtWidgets/QMainWindow>
#include <QHBoxlayout>
#include "ui_strangevismoviemaker.h"
#include "Renderer.h"
#include "histogram.h"


class strangevismoviemaker : public QMainWindow
{
    Q_OBJECT

public:
    strangevismoviemaker(Renderer* renderer, QWidget *parent = Q_NULLPTR);
public slots:
    void fileOpen();
    void highresScreenshot();
    void clearStates();
    void setBackgroundColor();
    void adjustAnimationDuration();
private:
    Ui::strangevismoviemakerClass ui;
    Renderer* m_renderer;
    void appendDockWidgets();
    void formatDockWidgets(QDockWidget* dw);
    QWidget* toolbarContent(QWidget*, QString);
    void closeEvent(QCloseEvent* event);
    QWidget* keyframeWrapper;
    QSize* square;
    int screenshotCount = 0;
    // class Histogram* m_histogram_widget{};
};
