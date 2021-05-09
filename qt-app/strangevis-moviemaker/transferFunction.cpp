#include "transferFunction.h"
#include <QOpenGLFunctions>
#include <QVector4D>
#include <QDebug>


TransferFunction::TransferFunction() : QObject() ,m_transferfunction(QOpenGLTexture::Target2D)
{
	for (int i = 0; i < 512 * 4; i++)
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
		m_transferfunction.setSize(512);
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

void TransferFunction::setSmoothingFactor(int smoothingFactor)
{
	if (smoothingFactor >= 0 && smoothingFactor <= 512)
	{
		m_smoothingFactor = smoothingFactor;

	}

	for (int i = 0; i < 512 * 4; i++)
	{
		if (i % 4 == 0 || ((i + 1) % 4 == 0))
			m_data << 1.0;
		else
			m_data << 0.0;
	}

	reloadLayers();
}

int TransferFunction::getSmoothingFactor()
{
	return m_smoothingFactor;
}

void TransferFunction::updateTransferFunction(int startIdx, int endIdx, QVector<float>* data) 
{
	// error checking
	if (startIdx >= endIdx)
	{
		qDebug() << "Start index must be smaller than end index";
		return;
	}
	if (startIdx < 0 || endIdx > 512)
	{
		qDebug() << "Index out of bounds";
		return;
	}

	m_updated = true;
	for (int i = startIdx * 4; i < (endIdx) * 4; i++)
	{
		m_data[i] = data->at(i - startIdx*4);
	}
	
	int startSmoothing = qMax(0, startIdx - m_smoothingFactor);
	for (int i = startSmoothing; i < startIdx; i++)
	{
		float t = float(i - (startIdx - m_smoothingFactor)) / float(m_smoothingFactor);
		m_data[i * 4] = (1.0 - t) * m_data[i * 4] + t * data->at((0));
		m_data[i * 4 + 1] = (1.0 - t) * m_data[i * 4 + 1] + t * data->at(1);
		m_data[i * 4 + 2] = (1.0 - t) * m_data[i * 4 + 2] + t * data->at(2);
		m_data[i * 4 + 3] = (1.0 - t) * m_data[i * 4 + 3] + t * data->at(3);
	}
	
	int endSmoothing = qMin(512, endIdx + m_smoothingFactor);
	for (int i = endIdx; i < endSmoothing; i++)
	{
		float t = float(i - endIdx) / float(m_smoothingFactor);
		m_data[i * 4] = (1.0 - t) * data->at(0) + t * m_data[i * 4];
		m_data[i * 4 + 1] = (1.0 - t) * data->at(1) + t * m_data[i * 4 + 1];
		m_data[i * 4 + 2] = (1.0 - t) * data->at(2) + t * m_data[i * 4 + 2];
		auto a = i * 4 + 3;
		m_data[i * 4 + 3] = (1.0 - t) * data->at(3) + t * m_data[i * 4 + 3];
	}
}

QVector<float> TransferFunction::getTransferFunctionData()
{
	return m_data;
}
