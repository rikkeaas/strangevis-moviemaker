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

private:
    Ui::strangevismoviemakerClass ui;
    Renderer* m_renderer;
    void appendDockWidgets();
    // class Histogram* m_histogram_widget{};
};
