
#pragma once

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QWidget>
#include <QVector3d>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include "model.h"
#include "histogram.h"
#include "keyframeHandler.h"
#include "interpolation.h"

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
	Renderer(QWidget* parent, Qt::WindowFlags f = Qt::WindowFlags());
	~Renderer();
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	Model* getVolume();
	QWidget* setKeyframes(QWidget*, QSize*);
	void setKeyframeWrapper(QWidget* qw);
	void clearStates();

public slots:
	void setMatrices(QList<QMatrix4x4> matrices);
	void addNewKeyframe();
private:
	QMatrix4x4 m_projectionMatrix;
	QMatrix4x4 m_rotateMatrix;
	QMatrix4x4 m_scaleMatrix;
	QMatrix4x4 m_translateMatrix;

	QList<QMatrix4x4> fromKeyframe;
	QList<QMatrix4x4> toKeyframe;

	QOpenGLShaderProgram shaderProgram;
	QVector<QVector3D> vertices;

	QOpenGLWidget* openglWidget;

	double alpha;
	double beta;
	double distance;
	QPoint lastMousePosition;

	bool m_rotating = false; // True if shift is held down, false otherwise. To allow users to rotate with trackpad/mouse while holding shift, otherwise trackpad/mouse is used for translation

	int m_div = 100;
	int m_prev = 100;
	float m_zCoord = 0.5;
	
	qreal m_currentX, m_currentY;
	qreal m_previousX, m_previousY;
	QVector3D arcballVector(qreal x, qreal y);

	Model* m_volume;
	KeyframeHandler* m_keyframeHandler;
	QWidget* keyframeWrapper;
	void setState();

	QSize* square;
	float t = 1;
	QElapsedTimer timer;
	LinearInterpolation* interpolater;

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
};
