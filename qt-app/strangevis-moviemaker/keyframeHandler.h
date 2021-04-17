#pragma once
#include <QFile>
#include <QOpenGLWidget>


class KeyframeHandler {
public:
	void saveState(QWidget*, QString, QList<float*>);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*);

private:
	void takeQtScreenShot(QWidget* widget, QString);
	int numberofStates;
};