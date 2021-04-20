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
