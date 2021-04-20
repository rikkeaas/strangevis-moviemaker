#pragma once
#include <QFile>
#include <QOpenGLWidget>
#include <QMatrix4x4>


class KeyframeHandler : public QWidget {
	Q_OBJECT
public:
	void saveState(QWidget* widget, QString filename, QList<float*> matrices);
	QWidget* updateKeyframes(QWidget* keyframeWrapper, QSize*, QString filename);
	void takeQtScreenShot(QWidget* widget, QString);
	bool toDelete = false;
public slots:
	void readStates(QString statePath);
	void deleteKeyframe(QString statePath, QString snapshotPath);
	void addButton();

signals:
	void matricesUpdated(QList<QMatrix4x4>);
	void addedKeyframe();
	void deletedKeyframe();
private:
	int numberofStates;
	int filenameNumber;
	void setFilenameNumber();
	QList<QStringList> getFiles();
	QString filename;
};
