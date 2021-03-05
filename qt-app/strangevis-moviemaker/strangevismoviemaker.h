#pragma once

#include <QtWidgets/QMainWindow>
#include <QHBoxlayout>
#include "ui_strangevismoviemaker.h"

class strangevismoviemaker : public QMainWindow
{
    Q_OBJECT

public:
    strangevismoviemaker(QWidget *parent = Q_NULLPTR);

public slots:
    void buttonClicked();

private:
    Ui::strangevismoviemakerClass ui;
};
