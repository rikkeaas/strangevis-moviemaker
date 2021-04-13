#include "phasefunction.h"
#include <QOpenGLFunctions>
#include <QVector4D>


PhaseFunction::PhaseFunction() : m_phasefunction(QOpenGLTexture::Target2D)
{
}


void PhaseFunction::bind()
{
	if (!m_phasefunction.isCreated())
	{
		initializeOpenGLFunctions();

		m_phasefunction.setBorderColor(0, 0, 0, 0);
		m_phasefunction.setWrapMode(QOpenGLTexture::ClampToBorder);
		m_phasefunction.setFormat(QOpenGLTexture::RGBA32F);
		m_phasefunction.setMinificationFilter(QOpenGLTexture::Linear);
		m_phasefunction.setMagnificationFilter(QOpenGLTexture::Linear);
		m_phasefunction.setAutoMipMapGenerationEnabled(false);
		m_phasefunction.setSize(5);
		m_phasefunction.allocateStorage();

		QVector<float> data;
		data << 1.0 << 0.0 << 0.0 << 0.0
			<< 1.0 << 0.3 << 0.3 << 0.0
			<< 1.0 << 1.0 << 0.0 << 0.2
			<< 0.0 << 0.0 << 1.0 << 0.2
			<< 1.0 << 1.0 << 1.0 << 0.5;

		/*
		data.append(QVector4D(0.0, 0.0, 0.0, 0.0));
		data.append(QVector4D(0.0, 1.0, 0.0, 0.05));
		data.append(QVector4D(1.0, 0.0, 0.0, 0.05));
		data.append(QVector4D(1.0, 0.0, 0.0, 0.05));
		data.append(QVector4D(1.0, 0.0, 0.0, 0.05));
		
		data.append(QVector4D(0, 0.0, 0.0, 0.02));
		data.append(QVector4D(0.2, 0.0, 0.0, 0.03));
		data.append(QVector4D(0.2, 0.0, 0.0, 0.04));
		data.append(QVector4D(1.0, 0.0, 0.0, 0.05));
		*/

		//void* data = reinterpret_cast<void*>(m_Data.data());
		m_phasefunction.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, data.data());
	}

	m_phasefunction.bind();
}

void PhaseFunction::release()
{
	m_phasefunction.release();
}