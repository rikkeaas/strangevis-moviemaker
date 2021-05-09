#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>

class TransferFunction : public QObject, protected QOpenGLExtraFunctions
{
	Q_OBJECT
public:
	TransferFunction();
	~TransferFunction() {};

	void bind();
	void release();

	void updateTransferFunction(int startIdx, int endIdx, QVector<float>* data);
	QVector<float> getTransferFunctionData();
	void setSmoothingFactor(int smoothingFactor);
	int getSmoothingFactor();
signals:
	void reloadLayers();
private:
	QOpenGLTexture m_transferfunction;

	int m_smoothingFactor = 10;
	QVector<float> m_data;
	bool m_updated = false;
};