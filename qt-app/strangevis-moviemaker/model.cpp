#include "model.h"
#include <stdio.h>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QDataStream>
#include <QVector3D>

Model::Model(QObject* parent) : QObject(parent), m_volumeTexture(QOpenGLTexture::Target3D)
{}

Model::~Model()
{}

bool Model::load(const QString filepath)
{
	// Opening file
	qDebug() << "Loading " << filepath << "...";
	QFile file(filepath);
	QFile metaFile(filepath.left(filepath.length() - 3) + "ini");

	if (!metaFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Couldnt open " << filepath.left(filepath.length() - 3) + "ini";
		qDebug() << "Setting voxel spacing to 1,1,1";
		voxelSpacing = QVector3D(1.0, 1.0, 1.0);
	}
	else
	{
		QTextStream metaStream(&metaFile);
		metaStream.readLine(); // Reading title line (not needed)
		for (int i = 0; i < 3; i++)
		{
			QString s = metaStream.readLine();
			int numLen = s.length() - s.lastIndexOf("=") - 1;
			qDebug() << i << " " << s.right(numLen);
			voxelSpacing[i] = s.right(numLen).toFloat();
		}
	}

	voxelSpacing /= qMax(qMax(voxelSpacing[0], voxelSpacing[1]), voxelSpacing[2]);

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
	m_Data.resize(volumeSize*4);
	m_width = uWidth;
	m_height = uHeight;
	m_depth = uDepth;

	dimensionScaling = QVector3D(uWidth, uHeight, uDepth);
	dimensionScaling /= qMax(qMax(uWidth, uHeight), uDepth);

	m_updateNeeded = true;

	for (long i = 0; i < volumeSize; i++)
	{
		m_Data[i*4] = data[i] * 16; // Upscaling values from 12bit to 16bit range
	}

	/*
	qDebug() << "Computing gradients";

	for (long z = 0; z < uDepth; z++) 
	{
		long nextZ = z == 0 ? 0 : z - 1;
		long prevZ = z + 1 == uDepth ? z : z + 1;
		for (long y = 0; y < uHeight; y++) 
		{
			long nextY = y == 0 ? 0 : y - 1;
			long prevY = y + 1 == uHeight ? y : y + 1;
			for (long x = 0; x < uWidth; x++) 
			{
				long nextX = x == 0 ? 0 : x - 1;
				long prevX = x + 1 == uWidth ? x : x + 1;

				//QVector3D gradient = QVector3D(0.5 * (m_Data[4 * (nextX + uWidth * (y + z * uHeight))] - m_Data[4 * (prevX + uWidth * (y + z * uHeight))]),
				//							   0.5 * (m_Data[4 * (x + uWidth * (nextY + z * uHeight))] - m_Data[4 * (x + uWidth * (prevY + z * uHeight))]),
				//							   0.5 * (m_Data[4 * (x + uWidth * (y + nextZ * uHeight))] - m_Data[4 * (x + uWidth * (y + prevZ * uHeight))]));

				QVector3D gradient = QVector3D(0.5 * (data[(nextX + uWidth * (y + z * uHeight))] - data[(prevX + uWidth * (y + z * uHeight))]),
					0.5 * (data[(x + uWidth * (nextY + z * uHeight))] - data[(x + uWidth * (prevY + z * uHeight))]),
					0.5 * (data[(x + uWidth * (y + nextZ * uHeight))] - data[(x + uWidth * (y + prevZ * uHeight))]));

				m_Data[1 + 4*(x + uWidth * (y + z * uHeight))] = gradient.x();
				m_Data[2 + 4*(x + uWidth * (y + z * uHeight))] = gradient.y();
				m_Data[3 + 4*(x + uWidth * (y + z * uHeight))] = gradient.z();
			}
		}
	}
	*/
	
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
		m_volumeTexture.setFormat(QOpenGLTexture::RGBA16F);
		m_volumeTexture.setMinificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture.setMagnificationFilter(QOpenGLTexture::Linear);
		m_volumeTexture.setAutoMipMapGenerationEnabled(false);
		m_volumeTexture.setSize(m_width, m_height, m_depth);
		m_volumeTexture.allocateStorage();

		//void* data = reinterpret_cast<void*>(m_Data.data());
		m_volumeTexture.setData(0, 0, 0, m_width, m_height, m_depth, QOpenGLTexture::RGBA, QOpenGLTexture::UInt16, m_Data.data());
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


QVector3D Model::getDimensions() 
{
	return QVector3D(m_width, m_height, m_depth);
}


QVector3D Model::getVoxelSpacing() { return voxelSpacing; }

QVector3D Model::getDimensionScale() { return dimensionScaling; }