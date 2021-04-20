#include "keyframes.h"
#include <QStyleOption>
#include <QPainter>
#include <QKeyEvent>


Keyframe::Keyframe(KeyframeHandler* keyframeHandler) : QWidget(keyframeHandler)
{
	this->keyframeHandler = keyframeHandler;
}

void Keyframe::setStatePath(QString statePath)
{
	this->statePath = statePath;
}

void Keyframe::setSnapshotPath(QString snapshotPath)
{
	this->snapshotPath = snapshotPath;
}

void Keyframe::mousePressEvent(QMouseEvent* event)
{
	if (keyframeHandler->toDelete) {
		clickForRemove(statePath, snapshotPath);
	} else {
		clicked(statePath);
	}
}

// some hacky code from Stack Overflow
// https://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
void Keyframe::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, & p, this);
}

AddButton::AddButton(KeyframeHandler* keyframeHandler) : QWidget(keyframeHandler) {
	this->keyframeHandler = keyframeHandler;
}

void AddButton::mousePressEvent(QMouseEvent* event)
{
	clicked();
}

// some hacky code from Stack Overflow
// https://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget
void AddButton::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
