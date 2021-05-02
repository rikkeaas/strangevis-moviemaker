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
    void cut();
    void setRadius();
    void setCutSize();
    void setShowCut();
    void setTypeOfAnimation();
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
    int m_cutType = 0;
    double m_cutRadius = 0.5;
    double m_cutSize = 0.5;
    QMenu* cutMenu;
    int m_showCut = 0;
    int m_animationType = 0;
    // class Histogram* m_histogram_widget{};
};
