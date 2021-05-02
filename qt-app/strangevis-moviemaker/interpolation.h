#pragma once
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QQuaternion>


class LinearInterpolation {
public:
	QList<QMatrix4x4> interpolate(QList<QMatrix4x4>, QList<QMatrix4x4>, float);
	QVector3D backgroundInterpolation(QVector3D, QVector3D, float);
	QVector<float> transferFunctionInterpolation(QVector<float> fromColor, QVector<float> toColor, float f);
};

class CatmullRomInterpolation // For animation
{
public:
	static QVector<float> transferFunctionInterpolation(QVector<float> prevColor, QVector<float> fromColor, QVector<float> toColor, QVector<float> nextColor, float f);
	static QVector3D vectorInterpolation(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, float samplePoint);
	static QList<QMatrix4x4> matrixInterpolation(QList<QMatrix4x4> p0, QList<QMatrix4x4> p1, QList<QMatrix4x4> p2, QList<QMatrix4x4> p3, float samplePoint);
private:
	static float scalarInterpolation(float p0, float p1, float p2, float p3, float samplePoint);
	static QVector3D extractScale(QMatrix4x4 matrix);
	static QVector3D extractTranslation(QMatrix4x4 matrix);
	static QMatrix4x4 removeTranslation(QMatrix4x4 matrix);
	static QMatrix3x3 extractRotation(QMatrix4x4 matrix);
	static QQuaternion interpolateRotation(QQuaternion r0, QQuaternion r1, QQuaternion r2, QQuaternion r3, float samplePoint);
	static QMatrix4x4 catmullRomMat();
	static QVector3D quaternionComponents(QQuaternion);

};