#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>
#include <QVector3d>
#include <vector>
#include <QFuture>
#include <QFutureWatcher>

class Volume {
public:
	QVector3D voxelSpacing;
	QVector3D dimensionScaling = QVector3D(1.0, 1.0, 1.0);

	// Model dimensions
	unsigned int m_height = 0;
	unsigned int m_width = 0;
	unsigned int m_depth = 0;

	bool m_updateNeeded = false;

	// Model data
	QVector<unsigned short> m_Data;
	QString modelFilename;
	Volume(QVector3D voxelSpacing, QVector3D dimensionScaling, unsigned int m_height, unsigned int m_width, unsigned int m_depth, bool m_updateNeeded, QVector<unsigned short> m_Data, QString modelFilename);
};

class Model : public QObject, protected QOpenGLExtraFunctions
{
	Q_OBJECT
public:
	Model(QObject* parent);
	~Model();

	QVector3D getDimensions();
	QVector3D getVoxelSpacing();
	QVector3D getDimensionScale();

	void threadedLoading(const QString filepath);
	Volume* load(const QString filepath);
	void bind();
	void release();

	QVector<unsigned short> getDataset();
	QString getFilename();
signals:
	void loadedModel();
public slots:
	void setIncomingVolume();
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
	QString modelFilename;

	QFuture<Volume*> m_future;
	QFutureWatcher<Volume*> m_watcher;

};

