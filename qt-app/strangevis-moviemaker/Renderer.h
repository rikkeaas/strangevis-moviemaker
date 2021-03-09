
#pragma once

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QWidget>
#include <QVector3d>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>

#include "model.h"

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
	Renderer();
	~Renderer();
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
private:
	bool myFunc(QWidget* f);
	QMatrix4x4 pMatrix;
	QOpenGLShaderProgram shaderProgram;
	QVector<QVector3D> vertices;

	QOpenGLWidget* openglWidget;
	std::unique_ptr<Model> m_model;

	double alpha;
	double beta;
	double distance;
	QPoint lastMousePosition;

	int m_div = 50;
	int m_prev = 50;
	void createTexture();

	GLuint m_textureID;
protected:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
};
