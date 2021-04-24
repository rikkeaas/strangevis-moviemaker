#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>

class PhaseFunction : protected QOpenGLExtraFunctions
{
public:
	PhaseFunction();
	~PhaseFunction() {};

	void bind();
	void release();

	void updatePhaseFunction(int startIdx, int endIdx, QVector<float>* data);
private:
	QOpenGLTexture m_phasefunction;

	QVector<float> m_data;
	bool m_updated = false;


};