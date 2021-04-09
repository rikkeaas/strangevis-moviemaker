#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>
#include <QVector3d>

class Model : public QObject, protected QOpenGLExtraFunctions
{
Q_OBJECT
public:
	Model(QObject* parent);
	~Model();

	QVector3D getDimensions();
	QVector3D getVoxelSpacing();
	QVector3D getDimensionScale();

	bool load(const QString filepath);
	void bind();
	void release();

	QVector<unsigned short> getDataset();

private:
	QVector3D voxelSpacing;
	QVector3D dimensionScaling = QVector3D(1.0,1.0,1.0);

	// Model dimensions
	unsigned int m_height = 0;
	unsigned int m_width = 0;
	unsigned int m_depth = 0;

	bool m_updateNeeded = false;
	QOpenGLTexture m_volumeTexture;

	// Model data
	QVector<unsigned short> m_Data;

};