#pragma once
#include <QFile>
#include <QOpenGLWidget>
#include <QMatrix4x4>


class KeyframeHandler : public QWidget {
	Q_OBJECT
public:
	void saveState(QWidget* widget, QString filename, QList<float*> matrices);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*);
	void takeQtScreenShot(QWidget* widget, QString);
public slots:
	void readStates(QString statePath);
	void addButton();

signals:
	void matricesUpdated(QList<QMatrix4x4>);
	void addedKeyframe();
private:
	int numberofStates;
};
