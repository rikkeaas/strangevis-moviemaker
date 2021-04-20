#pragma once
#include <QMatrix4x4>
#include <QMatrix3x3>

class LinearInterpolation {
public:
	QList<QMatrix4x4> interpolate(QList<QMatrix4x4>, QList<QMatrix4x4>, float);
};