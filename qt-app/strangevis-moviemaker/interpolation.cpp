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
	for (int i = 0; i < fromColor.length(); i++) {
		float r = (toColor.at(i) - fromColor.at(i)) * f + fromColor.at(i);
		newColor << r;
	}
	return newColor;
}


QMatrix4x4 CatmullRomInterpolation::catmullRomMat()
{
	QMatrix4x4 catmullRomMat = QMatrix4x4(-1.0, 3.0, -3.0, 1.0, 2.0, -5.0, 4.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 2.0, 0.0, 0.0);
	catmullRomMat = 0.5 * catmullRomMat;
	return catmullRomMat;
}

float CatmullRomInterpolation::scalarInterpolation(float p0, float p1, float p2, float p3, float samplePoint)
{
	//vec4 samplePointPow = vec4(pow(samplePoint, 3), pow(samplePoint, 2), samplePoint, 1);
	//vec4 points = vec4(p0, p1, p2, p3);

	return 0;//dot((samplePointPow * bezierMat()), points);
}


QVector3D CatmullRomInterpolation::vectorInterpolation(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, float samplePoint)
{
	//vec4 samplePointPow = vec4(pow(samplePoint, 3), pow(samplePoint, 2), samplePoint, 1);
	//mat4x3 points = mat4x3(p0, p1, p2, p3);

	return QVector3D();//points * (samplePointPow * bezierMat());
}


QQuaternion CatmullRomInterpolation::interpolateRotation(QQuaternion r0, QQuaternion r1, QQuaternion r2, QQuaternion r3, float samplePoint)
{
	//quat temp1 = slerp(r0, r1, samplePoint);
	//quat temp2 = slerp(r1, r2, samplePoint);
	//quat temp3 = slerp(r2, r3, samplePoint);

	//quat temp4 = slerp(temp1, temp2, samplePoint);
	//quat temp5 = slerp(temp2, temp3, samplePoint);

	return QQuaternion();//slerp(temp4, temp5, samplePoint);
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
	interpolatedTranslation.translate(interpolatedTranslationVec);

	// Exctracting scale from keyframe transform matrices, interpolating, then getting the scale matrix with the interpolated scale
	QVector3D s0 = extractScale(p0[2]);
	QVector3D s1 = extractScale(p1[2]);
	QVector3D s2 = extractScale(p2[2]);
	QVector3D s3 = extractScale(p3[2]);
	QVector3D interpolatedScaleVec = CatmullRomInterpolation::vectorInterpolation(s0, s1, s2, s3, samplePoint);
	QMatrix4x4 interpolatedScale;
	interpolatedScale.setToIdentity();
	interpolatedScale.scale(interpolatedScaleVec);

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





QVector3D CatmullRomInterpolation::extractScale(QMatrix4x4 matrix)
{
	/*
	vec3 scale;
	for (int i = 0; i < 3; i++)
	{
		scale[i] = length(vec3(matrix[i]));
	}
	return scale;
	*/
	return QVector3D();
}


QVector3D CatmullRomInterpolation::extractTranslation(QMatrix4x4 matrix)
{
	//return vec3(matrix[3]);
	return QVector3D();
}


QMatrix4x4 CatmullRomInterpolation::removeTranslation(QMatrix4x4 matrix)
{
	/*
	mat4 newMat = matrix;
	for (int i = 0; i < 3; i++)
	{
		newMat[3][i] = 0;
	}
	newMat[3][3] = 1;
	return newMat;
	*/
	return QMatrix4x4();
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