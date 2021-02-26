#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_strangevismoviemaker.h"

class strangevismoviemaker : public QMainWindow
{
    Q_OBJECT

public:
    strangevismoviemaker(QWidget *parent = Q_NULLPTR);

private:
    Ui::strangevismoviemakerClass ui;
};
