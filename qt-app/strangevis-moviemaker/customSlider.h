#pragma once

#include <QtWidgets/QWidget>
#include "ui_strangevismoviemaker.h"

class customSlider : public QWidget
{
    Q_OBJECT

public:
    customSlider(int minValue, int maxValue, QWidget* parent = Q_NULLPTR);
    ~customSlider();

signals:

public slots:

};
