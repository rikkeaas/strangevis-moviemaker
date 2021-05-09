#include "interpolation.h"


// ---------------------------------------------------------------------------------------------------------------------
// LINEAR INTERPOLATION
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

QVector<float> LinearInterpolation::transferFunctionInterpolation(QVector<float> fromColor, QVector<float> toColor, float f)
{
	QVector<float> newColor;
	for (int i = 0; i < fromColor.length(); i++) {
		if (fromColor[i] <= 0.0001 && toColor[i] <= 0.0001)
		{
			newColor << 0.0;
		}
		else
		{
			float r = (toColor.at(i) - fromColor.at(i)) * f + fromColor.at(i);
			newColor << r;
		}
	}
	return newColor;
}
// ---------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------------------------
// CATMULL-ROM SPLINE INTERPOLATION
QMatrix4x4 CatmullRomInterpolation::catmullRomMat()
{
	QMatrix4x4 catmullRomMat = QMatrix4x4(-1.0, 3.0, -3.0, 1.0, 2.0, -5.0, 4.0, -1.0,-1.0, 0.0, 1.0, 0.0, 0.0, 2.0, 0.0, 0.0);
	return 0.5 * catmullRomMat;
}

float CatmullRomInterpolation::scalarInterpolation(float p0, float p1, float p2, float p3, float samplePoint)
{
	float r = (p2 - p1) * samplePoint + p1;
	return r;
}


QVector3D CatmullRomInterpolation::vectorInterpolation(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, float samplePoint)
{
	QVector4D samplePointPow = QVector4D(pow(samplePoint, 3), pow(samplePoint, 2), samplePoint, 1);
	QMatrix4x4 points;
	points.setRow(0, QVector4D(p0, 0.0));
	points.setRow(1, QVector4D(p1, 0.0));
	points.setRow(2, QVector4D(p2, 0.0));
	points.setRow(3, QVector4D(p3, 0.0));

	return ((samplePointPow * catmullRomMat()) * points).toVector3D();
}

QVector3D CatmullRomInterpolation::quaternionComponents(QQuaternion q)
{
	QVector3D quatComp;
	quatComp.setX(q.x());
	quatComp.setY(q.y());
	quatComp.setZ(q.z());
	return quatComp;
}

QQuaternion CatmullRomInterpolation::interpolateRotation(QQuaternion r0, QQuaternion r1, QQuaternion r2, QQuaternion r3, float samplePoint)
{
	QVector3D qr0 = quaternionComponents(r0);
	QVector3D qr1 = quaternionComponents(r1);
	QVector3D qr2 = quaternionComponents(r2);
	QVector3D qr3 = quaternionComponents(r3);
	
	QVector3D interQ = vectorInterpolation(qr0, qr1, qr2, qr3, samplePoint);

	float interAng = scalarInterpolation(r0.scalar(), r1.scalar(), r2.scalar(), r3.scalar(), samplePoint);
	return QQuaternion(interAng, interQ).normalized();
}


QList<QMatrix4x4> CatmullRomInterpolation::matrixInterpolation(QList<QMatrix4x4> p0, QList<QMatrix4x4> p1, QList<QMatrix4x4> p2, QList<QMatrix4x4> p3, float samplePoint)
{

	// Extracting translation from keyframe transform matrices, interpolating, then getting the translation matrix with the interpolated translation
	QVector3D t0 = extractTranslation(p0[3]);
	QVector3D t1 = extractTranslation(p1[3]);
	QVector3D t2 = extractTranslation(p2[3]);
	QVector3D t3 = extractTranslation(p3[3]);
	QVector3D interpolatedTranslationVec = CatmullRomInterpolation::vectorInterpolation(t0, t1, t2, t3, samplePoint);
	QMatrix4x4 interpolatedTranslation;
	interpolatedTranslation.setToIdentity();
	interpolatedTranslation.setColumn(3, QVector4D(interpolatedTranslationVec, 1.0));

	// Exctracting scale from keyframe transform matrices, interpolating, then getting the scale matrix with the interpolated scale
	QVector3D s0 = extractScale(p0[2]);
	QVector3D s1 = extractScale(p1[2]);
	QVector3D s2 = extractScale(p2[2]);
	QVector3D s3 = extractScale(p3[2]);
	QVector3D interpolatedScaleVec = CatmullRomInterpolation::vectorInterpolation(s0, s1, s2, s3, samplePoint);
	QMatrix4x4 interpolatedScale;
	interpolatedScale.setToIdentity();
	interpolatedScale(0, 0) = interpolatedScaleVec[0];
	interpolatedScale(1, 1) = interpolatedScaleVec[1];
	interpolatedScale(2, 2) = interpolatedScaleVec[2];
	//interpolatedScale.scale(interpolatedScaleVec);

	// Extracting rotation from keyframe transform matrices, interpolating with slerp, then getting the interpolated rotation
	QMatrix3x3 rot0 = extractRotation(p0[1]);
	QMatrix3x3 rot1 = extractRotation(p1[1]);
	QMatrix3x3 rot2 = extractRotation(p2[1]);
	QMatrix3x3 rot3 = extractRotation(p3[1]);
	QMatrix3x3 interpolatedRot = (CatmullRomInterpolation::interpolateRotation(QQuaternion::fromRotationMatrix(rot0), QQuaternion::fromRotationMatrix(rot1), QQuaternion::fromRotationMatrix(rot2), QQuaternion::fromRotationMatrix(rot3), samplePoint)).toRotationMatrix();

	QMatrix4x4 rotMat;
	rotMat.setToIdentity();
	for (int i = 0; i < 3; i++)
	{
		QVector4D col;
		for (int j = 0; j < 3; j++)
		{
			col[j] = interpolatedRot(j, i);
		}
		rotMat.setColumn(i, col);
	}


	QList<QMatrix4x4> m_out;
	m_out.append(p1[0]);
	m_out.append(rotMat);
	m_out.append(interpolatedScale);
	m_out.append(interpolatedTranslation);
	
	return m_out;
}

QVector<float> CatmullRomInterpolation::transferFunctionInterpolation(QVector<float> prevColor, QVector<float> fromColor, QVector<float> toColor, QVector<float> nextColor, float f)
{
	QVector<float> outTF;
	for (int i = 0; i < fromColor.length(); i++) {
		float r = scalarInterpolation(prevColor[i], fromColor[i], toColor[i], nextColor[i], f);
		outTF << r;
		
	}
	return outTF;
}

QVector3D CatmullRomInterpolation::extractScale(QMatrix4x4 matrix)
{
	QVector3D scale;
	for (int i = 0; i < 3; i++)
	{
		scale[i] = matrix(i,i);
	}
	return scale;
}

QVector3D CatmullRomInterpolation::extractTranslation(QMatrix4x4 matrix)
{
	return matrix.column(3).toVector3D();
}

QMatrix3x3 CatmullRomInterpolation::extractRotation(QMatrix4x4 matrix)
{
	QMatrix3x3 rotation;
	rotation.setToIdentity();
	
	for (int i = 0; i < 3; i++)
	{
		QVector3D row = QVector3D(matrix.row(i));
		for (int j = 0; j < 3; j++)
		{
			rotation(i, j) = row[j];
		}
	}
	return rotation;
}
// ---------------------------------------------------------------------------------------------------------------------