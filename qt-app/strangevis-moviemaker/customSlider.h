#pragma once

#include <QtWidgets/QWidget>
#include <QSlider>
#include <QSpinBox>
#include "ui_strangevismoviemaker.h"


class CustomSlider : public QWidget
{
    Q_OBJECT

public:
    CustomSlider(int color, QRect sliderPos, int minValue, int maxValue, int currValue, QWidget* parent);
    ~CustomSlider();
    
signals:
    void valueChanged(int newVal);
public slots:
    void setPosition(int newVal);
    void setValue(int val);
    void sendValueSignal(int val);

private:
    QSpinBox* m_spinbox;
    QSlider* m_horizontalSlider;

};
