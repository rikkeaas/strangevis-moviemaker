
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
#include <QElapsedTimer>

#include "model.h"
#include "keyframeHandler.h"
#include "phasefunction.h"
#include "layer.h";

#include "interpolation.h"

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
	Renderer();
	Renderer(QWidget* parent, Qt::WindowFlags f = Qt::WindowFlags());
	~Renderer();
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	Model* getVolume();
	QWidget* setKeyframes(QWidget*, QSize*);
	void setKeyframeWrapper(QWidget* qw);
	PhaseFunction* getPhaseFunction();
	void clearStates();
	void setBackgroundColor();
	void playAnimation();
	int getAnimationDuration();
	void setAnimationDuration(double);
	void setLayers(QList<Layer*>);
	void setSphereCut(bool);
	void setCubeCut(bool);
	void setSphereRadius(double);
	void setCubeSize(double);
	void setShowCut(bool, bool);
public slots:
	void setMatrices(QList<QMatrix4x4> matrices, QVector3D backgroundColor, QVector<float> phaseFunction, QList<Layer*> layers);
	void addNewKeyframe();
	void updateKeyframes();
	void toggleLightVolumeTransformation();
	void updateWidget();
signals:
	void updateLayers(QList<Layer*> layers);
private:
	QMatrix4x4 m_projectionMatrix;
	QMatrix4x4 m_rotateMatrix;
	QMatrix4x4 m_scaleMatrix;
	QMatrix4x4 m_translateMatrix;

	QMatrix4x4 m_lightRotateMatrix;
	QMatrix4x4 m_lightTranslateMatrix;
	bool m_transformLight = false;

	QVector3D m_backgroundColor;
	QVector<float> m_phaseFunctionData;

	QList<QMatrix4x4> fromKeyframe;
	QList<QMatrix4x4> toKeyframe;

	QVector3D fromBackgroundColor;
	QVector3D toBackgroundColor;

	QVector<float> fromPhaseFunction;
	QVector<float> toPhaseFunction;

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
	PhaseFunction* m_phasefunction;

	int clicks = 0;

	KeyframeHandler* m_keyframeHandler;
	QWidget* keyframeWrapper;
	void setState();

	QSize* square;
	float t = 1;
	QElapsedTimer timer;
	LinearInterpolation* interpolater;
	float animationDuration = 1000.f;

	bool m_sphereCut = false;
	bool m_cubeCut = false;
	double m_sphereCutRadius;
	double m_cubeCutSize;
	bool m_showCut = false;
	bool m_showInFront = false;

	bool isInterpolating = false;
	QList<Layer*> m_layers;

protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
};
