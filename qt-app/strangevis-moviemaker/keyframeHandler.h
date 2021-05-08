#pragma once
#include <QFile>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include "layer.h"


class KeyframeHandler : public QWidget {
	Q_OBJECT
public:
	void saveState(QWidget* widget, QString filename, QList<float*> matrices, QVector3D backgroundColor, QVector<float> phaseFunctionData, QList<Layer*> layers);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*, QString filename);
	void highlightKeyframe(QWidget* keyframeWrapper, int index);
	void removeKeyframeHighlighting(QWidget* keyframeWrapper, int index);
	void takeQtScreenShot(QWidget* widget, QString);
	bool toDelete = false;
	QList<QStringList> getFiles();
	void getStates(QString statePath, QList<QMatrix4x4> &matrices, QVector3D &background, QVector<float> &transferFunc, QList<Layer*> &layers);
public slots:
	void readStates(QString statePath);
	void deleteKeyframe(QString statePath, QString snapshotPath);
	void addButton();

signals:
	void matricesUpdated(QList<QMatrix4x4>, QVector3D, QVector<float>, QList<Layer*>);
	void addedKeyframe();
	void deletedKeyframe();
private:
	int numberofStates;
	int filenameNumber;
	void setFilenameNumber();
	QString filename;
	QString backupStyleSheet;
};
