#pragma once
#include <QMatrix4x4>
#include <QMatrix3x3>

class LinearInterpolation {
public:
	QList<QMatrix4x4> interpolate(QList<QMatrix4x4>, QList<QMatrix4x4>, float);
	QVector3D backgroundInterpolation(QVector3D, QVector3D, float);
	QVector<float> phaseFunctionInterpolation(QVector<float> fromColor, QVector<float> toColor, float f);
};