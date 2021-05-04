#pragma once
#include <QOpenGLTexture>
#include <QOpenGLExtraFunctions>

class TransferFunction : protected QOpenGLExtraFunctions
{
public:
	TransferFunction();
	~TransferFunction() {};

	void bind();
	void release();

	void updateTransferFunction(int startIdx, int endIdx, QVector<float>* data);
	QVector<float> getTransferFunctionData();
private:
	QOpenGLTexture m_transferfunction;

	QVector<float> m_data;
	bool m_updated = false;


};