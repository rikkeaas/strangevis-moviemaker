#include "model.h"
#include "histogram.h"
#include "strangevismoviemaker.h"
#include <stdio.h>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QDataStream>
#include <QVector3D>
#include <QDockWIdget>
#include <QtConcurrent>


Model::Model(QObject* parent) : QObject(parent), m_volumeTexture(QOpenGLTexture::Target3D)
{}

Model::~Model()
{}

void Model::threadedLoading(const QString filepath)
{
	m_future = QtConcurrent::run(this, &Model::load, filepath);
	m_watcher.setFuture(m_future);
	connect(&m_watcher, &QFutureWatcher<Volume*>::finished, this, &Model::setIncomingVolume);
}

void Model::setIncomingVolume() {
	Volume* vol = m_future.result();
	voxelSpacing = vol->voxelSpacing;
	dimensionScaling = vol->dimensionScaling;
	m_height = vol->m_height;
	m_width = vol->m_width;
	m_depth = vol->m_depth;
	m_updateNeeded = vol->m_updateNeeded;
	m_Data = vol->m_Data;
	modelFilename = vol->modelFilename;
	loadedModel();
}

Volume* Model::load(const QString filepath)
{
	// Opening file
	qDebug() << "Loading " << filepath << "...";
	QFile file(filepath);
	QFile metaFile(filepath.left(filepath.length() - 3) + "ini");

	QStringList fp = filepath.split("/");

	QString m_new_modelFilename = fp.at(fp.size()-1).split(".").at(0);

	qDebug() << m_new_modelFilename;

	QVector3D m_new_voxelSpacing;

	if (!metaFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Couldnt open " << filepath.left(filepath.length() - 3) + "ini";
		qDebug() << "Setting voxel spacing to 1,1,1";
		m_new_voxelSpacing = QVector3D(1.0, 1.0, 1.0);
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
			m_new_voxelSpacing[i] = s.right(numLen).toFloat();
		}
	}

	m_new_voxelSpacing /= qMax(qMax(m_new_voxelSpacing[0], m_new_voxelSpacing[1]), m_new_voxelSpacing[2]);

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Couldnt open " << filepath;
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
	}

	QVector<unsigned short> m_new_data;
	unsigned int m_new_width;
	unsigned int m_new_height;
	unsigned int m_new_depth;
	QVector3D m_new_dimensionScaling = QVector3D(1.0, 1.0, 1.0);
	bool m_new_updateNeeded = false;

	// Setting member variables
	m_new_data.resize(volumeSize);
	//m_Data.resize(volumeSize);
	m_new_width = uWidth;
	m_new_height = uHeight;
	m_new_depth = uDepth;

	m_new_dimensionScaling = QVector3D(uWidth, uHeight, uDepth);
	m_new_dimensionScaling /= qMax(qMax(uWidth, uHeight), uDepth);

	m_new_updateNeeded = true;

	for (long i = 0; i < volumeSize; i++)
	{
		m_new_data[i] = data[i] * 16; // Upscaling values from 12bit to 16bit range
	}
	
	qDebug() << "Successfully loaded " << filepath;

	Volume* vol = new Volume(m_new_voxelSpacing, m_new_dimensionScaling, m_new_height, m_new_width, m_new_depth, m_new_updateNeeded, m_new_data, m_new_modelFilename);

	return vol;
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

		//void* data = reinterpret_cast<void*>(m_Data.data());
		m_volumeTexture.setData(0, 0, 0, m_width, m_height, m_depth, QOpenGLTexture::Red, QOpenGLTexture::UInt16, m_Data.data());
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

QString Model::getFilename()
{
	return modelFilename;
}


QVector3D Model::getDimensions() 
{
	return QVector3D(m_width, m_height, m_depth);
}


QVector3D Model::getVoxelSpacing() { return voxelSpacing; }

QVector3D Model::getDimensionScale() { return dimensionScaling; }

Volume::Volume(QVector3D voxelSpacing, QVector3D dimensionScaling, unsigned int m_height, unsigned int m_width, unsigned int m_depth, bool m_updateNeeded, QVector<unsigned short> m_Data, QString modelFilename) {
	this->voxelSpacing = voxelSpacing;
	this->dimensionScaling = dimensionScaling;

	this->m_height = m_height;
	this->m_width = m_width;
	this->m_depth = m_depth;

	this->m_updateNeeded = m_updateNeeded;

	this->m_Data = m_Data;
	this->modelFilename = modelFilename;

}