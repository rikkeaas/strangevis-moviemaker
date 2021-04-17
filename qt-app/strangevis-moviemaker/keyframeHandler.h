#pragma once
#include <QFile>
#include <QOpenGLWidget>


class KeyframeHandler {
public:
	void saveState(QWidget* widget, QString filename, QList<float*> matrices);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*);
	QList<QMatrix4x4> readStates(int index);
	void takeQtScreenShot(QWidget* widget, QString);

private:
	int numberofStates;
};