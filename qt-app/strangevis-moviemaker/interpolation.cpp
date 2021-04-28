#include "interpolation.h"

QList<QMatrix4x4> LinearInterpolation::interpolate(QList<QMatrix4x4> old_matrices, QList<QMatrix4x4> new_matrices, float t)
{
	QMatrix4x4 old_rotateMatrix = old_matrices[1];
	QMatrix4x4 old_scaleMatrix = old_matrices[2];
	QMatrix4x4 old_translateMatrix = old_matrices[3];
	QMatrix4x4 new_projectionMatrix = old_matrices[0];
	QMatrix4x4 new_rotateMatrix = new_matrices[1];
	QMatrix4x4 new_scaleMatrix = new_matrices[2];
	QMatrix4x4 new_translateMatrix = new_matrices[3];

	QMatrix4x4 out_rotateMatrix;
	QMatrix4x4 out_scaleMatrix;
	QMatrix4x4 out_translateMatrix;

	out_scaleMatrix = (1.f - t) * old_scaleMatrix + (t * new_scaleMatrix);
	out_translateMatrix = (1.f - t) * old_translateMatrix + (t * new_translateMatrix);

	float old_matrix_float[9];
	float new_matrix_float[9];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			old_matrix_float[i * 3 + j] = old_rotateMatrix(i, j);
			new_matrix_float[i * 3 + j] = new_rotateMatrix(i, j);
		}
	}

	QMatrix3x3 inner_rotateMatrix = QMatrix3x3(old_matrix_float);
	QQuaternion fromRotate = QQuaternion::fromRotationMatrix(inner_rotateMatrix);

	inner_rotateMatrix = QMatrix3x3(new_matrix_float);
	QQuaternion toRotate = QQuaternion::fromRotationMatrix(inner_rotateMatrix);

	out_rotateMatrix = QMatrix4x4(QQuaternion::slerp(fromRotate, toRotate, t).toRotationMatrix());

	QList<QMatrix4x4> m_out;
	m_out.append(new_projectionMatrix);
	m_out.append(out_rotateMatrix);
	m_out.append(out_scaleMatrix);
	m_out.append(out_translateMatrix);

	return m_out;
}

QVector3D LinearInterpolation::backgroundInterpolation(QVector3D fromColor, QVector3D toColor, float f)
{
	float r = (toColor.x() - fromColor.x()) * f + fromColor.x();
	float g = (toColor.y() - fromColor.y()) * f + fromColor.y();
	float b = (toColor.z() - fromColor.z()) * f + fromColor.z();
	return QVector3D(r, g, b);
}

QVector<float> LinearInterpolation::phaseFunctionInterpolation(QVector<float> fromColor, QVector<float> toColor, float f)
{
	QVector<float> newColor;
	for (int i = 0; i < 1024; i+=4) {
		float r = (toColor.at(i) - fromColor.at(i)) * f + fromColor.at(i);
		float g = (toColor.at(i+1) - fromColor.at(i+1)) * f + fromColor.at(i+1);
		float b = (toColor.at(i+2) - fromColor.at(i+2)) * f + fromColor.at(i+2);
		float a = (toColor.at(i+3) - fromColor.at(i+3)) * f + fromColor.at(i+3);
		newColor << r << g << b << a;
	}
	return newColor;
}