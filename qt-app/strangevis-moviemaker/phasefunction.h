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
private:
	QOpenGLTexture m_phasefunction;


};