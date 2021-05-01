#pragma once
#include <QFile>
#include <QOpenGLWidget>
#include <QMatrix4x4>


class KeyframeHandler : public QWidget {
	Q_OBJECT
public:
	void saveState(QWidget* widget, QString filename, QList<float*> matrices, QVector3D backgroundColor, QVector<float> phaseFunctionData);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*, QString filename);
	void highlightKeyframe(QWidget* keyframeWrapper, int index);
	void removeKeyframeHighlighting(QWidget* keyframeWrapper, int index);
	void takeQtScreenShot(QWidget* widget, QString);
	bool toDelete = false;
	QList<QStringList> getFiles();
public slots:
	void readStates(QString statePath);
	void deleteKeyframe(QString statePath, QString snapshotPath);
	void addButton();

signals:
	void matricesUpdated(QList<QMatrix4x4>, QVector3D, QVector<float>);
	void addedKeyframe();
	void deletedKeyframe();
private:
	int numberofStates;
	int filenameNumber;
	void setFilenameNumber();
	QString filename;
	QString backupStyleSheet;
};
