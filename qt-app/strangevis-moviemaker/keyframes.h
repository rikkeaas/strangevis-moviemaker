#pragma once
#include <QWidget>
#include "keyframeHandler.h"

class Keyframe : public QWidget {
	Q_OBJECT
public:
	Keyframe(KeyframeHandler* keyframeHandler);
	void setStatePath(QString statePath);
	void setSnapshotPath(QString snapshotPath);
private:
	QString statePath;
	QString snapshotPath;
	bool toDelete = false;
	KeyframeHandler* keyframeHandler;

signals:
	void clicked(QString);
	void clickForRemove(QString, QString);
protected:
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};

class AddButton : public QWidget {
	Q_OBJECT
public:
	AddButton(KeyframeHandler* keyframeHandler);
private:
	KeyframeHandler* keyframeHandler;

signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
};