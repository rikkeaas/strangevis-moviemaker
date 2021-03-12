#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>

class Model : public QObject, protected QOpenGLExtraFunctions
{
Q_OBJECT
public:
	Model(QObject* parent);
	~Model();

	std::vector<unsigned short> getDimensions();

	bool load(const QString filepath);
	void bind();
	void release();

private:

	// Model dimensions
	unsigned int m_height = 0;
	unsigned int m_width = 0;
	unsigned int m_depth = 0;

	bool m_updateNeeded = false;
	QOpenGLTexture m_volumeTexture;

	// Model data
	QVector<unsigned short> m_Data;
};