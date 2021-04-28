#include "phasefunction.h"
#include <QOpenGLFunctions>
#include <QVector4D>
#include <QDebug>


PhaseFunction::PhaseFunction() : m_phasefunction(QOpenGLTexture::Target2D)
{
	for (int i = 0; i < 256 * 4; i++)
	{
		if (i % 4 == 0 || ((i + 1) % 4 == 0))
			m_data << 1.0;
		else
			m_data << 0.0;
	}
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
		m_phasefunction.setSize(256);
		m_phasefunction.allocateStorage();

		m_phasefunction.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, m_data.data());
	}

	if (m_updated)
	{
		m_updated = false;
		m_phasefunction.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, m_data.data());
	}

	m_phasefunction.bind();
}

void PhaseFunction::release()
{
	m_phasefunction.release();
}

void PhaseFunction::updatePhaseFunction(int startIdx, int endIdx, QVector<float>* data) 
{
	// error checking
	if (startIdx >= endIdx)
	{
		//qDebug() << "Start index must be smaller than end index";
		return;
	}
	if (startIdx < 0 || endIdx > 256)
	{
		qDebug() << "Index out of bounds";
		return;
	}

	m_updated = true;
	for (int i = startIdx * 4; i < (endIdx) * 4; i++)
	{
		m_data[i] = data->at((i - startIdx*4));
	}
}