#include "transferFunction.h"
#include <QOpenGLFunctions>
#include <QVector4D>
#include <QDebug>


TransferFunction::TransferFunction() : m_transferfunction(QOpenGLTexture::Target2D)
{
	for (int i = 0; i < 256 * 4; i++)
	{
		if (i % 4 == 0 || ((i + 1) % 4 == 0))
			m_data << 1.0;
		else
			m_data << 0.0;
	}
}


void TransferFunction::bind()
{
	if (!m_transferfunction.isCreated())
	{
		initializeOpenGLFunctions();

		m_transferfunction.setBorderColor(0, 0, 0, 0);
		m_transferfunction.setWrapMode(QOpenGLTexture::ClampToBorder);
		m_transferfunction.setFormat(QOpenGLTexture::RGBA32F);
		m_transferfunction.setMinificationFilter(QOpenGLTexture::Linear);
		m_transferfunction.setMagnificationFilter(QOpenGLTexture::Linear);
		m_transferfunction.setAutoMipMapGenerationEnabled(false);
		m_transferfunction.setSize(256);
		m_transferfunction.allocateStorage();

		m_transferfunction.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, m_data.data());
	}

	if (m_updated)
	{
		m_updated = false;
		m_transferfunction.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, m_data.data());
	}

	m_transferfunction.bind();
}

void TransferFunction::release()
{
	m_transferfunction.release();
}

void TransferFunction::updateTransferFunction(int startIdx, int endIdx, QVector<float>* data) 
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

QVector<float> TransferFunction::getTransferFunctionData()
{
	return m_data;
}
