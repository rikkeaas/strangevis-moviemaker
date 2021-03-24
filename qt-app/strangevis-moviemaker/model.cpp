#include "model.h"
#include <stdio.h>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QDataStream>

Model::Model(QObject* parent) : QObject(parent), m_volumeTexture(QOpenGLTexture::Target3D)
{}

Model::~Model()
{}

bool Model::load(const QString filepath)
{
	// Opening file
	qDebug() << "Loading " << filepath << "...";
	QFile file(filepath);

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Couldnt open " << filepath;
		return false;
	}

	QDataStream stream(&file);
	stream.setByteOrder(QDataStream::LittleEndian);

	// Reading dataset dimensions
	unsigned short uWidth, uHeight, uDepth;
	stream >> uWidth >> uHeight >> uDepth;

	qDebug() << "Width: " << uWidth << "\nHeight: " << uHeight << "\nDepth: " << uDepth;

	// Reading the data into a temporary vector
	int volumeSize = int(uWidth) * int(uHeight) * int(uDepth);
	QVector<unsigned short> data(volumeSize);
	if (stream.readRawData(reinterpret_cast<char*>(data.data()), volumeSize * sizeof(unsigned short)) != volumeSize * sizeof(unsigned short)) 
	{
		qDebug() << "Loading data failed";
		return false;
	}

	// Setting member variables
	m_Data.resize(volumeSize);
	m_width = uWidth;
	m_height = uHeight;
	m_depth = uDepth;
	m_updateNeeded = true;

	for (long i = 0; i < volumeSize; i++)
	{
		m_Data[i] = data[i] * 16; // Upscaling values from 12bit to 16bit range
	}

	qDebug() << "Successfully loaded " << filepath;

	return true;
}

void Model::bind()
{
	if (m_updateNeeded)
	{
		initializeOpenGLFunctions();

		qDebug() << "Binding";

		if (m_volumeTexture.isCreated())
			m_volumeTexture.destroy();

		m_volumeTexture.setBorderColor(0, 0, 0, 0);
		m_volumeTexture.setWrapMode(QOpenGLTexture::ClampToBorder);
		m_volumeTexture.setFormat(QOpenGLTexture::R16F);
		m_volumeTexture.setMinificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture.setMagnificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture.setAutoMipMapGenerationEnabled(false);
		m_volumeTexture.setSize(m_width, m_height, m_depth);
		m_volumeTexture.allocateStorage();

		void* data = reinterpret_cast<void*>(m_Data.data());
		m_volumeTexture.setData(0, 0, 0, m_width, m_height, m_depth, QOpenGLTexture::Red, QOpenGLTexture::UInt16, data);
		m_updateNeeded = false;
	}

	if (m_volumeTexture.isCreated())
	{
		m_volumeTexture.bind();
	}
}

void Model::release()
{
	if (m_volumeTexture.isCreated())
	{
		m_volumeTexture.release();
	}
}

QVector<unsigned short> Model::getDataset()
{
	QVector<unsigned short> copied = m_Data;
	return copied;
}


std::vector<unsigned short> Model::getDimensions() 
{
	std::vector<unsigned short> dims;
	dims.push_back(m_width);
	dims.push_back(m_height);
	dims.push_back(m_depth);
	return dims;
}
