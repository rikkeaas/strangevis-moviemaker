#pragma once

#include <QtWidgets/QWidget>
#include <QSlider>
#include <QSpinBox>
#include "ui_strangevismoviemaker.h"


class customSlider : public QWidget
{
    Q_OBJECT

public:
    customSlider(int color, QRect sliderPos, int minValue, int maxValue, QWidget* parent);
    ~customSlider();

signals:

public slots:
    void setPosition(int newVal);

private:
    QSpinBox* m_spinbox;
    QSlider* m_horizontalSlider;

};
