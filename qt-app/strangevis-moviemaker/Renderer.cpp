
#include "Renderer.h"
#include "cube.h"
#include <QtMath>

Renderer::Renderer(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f)
{
	setFocusPolicy(Qt::StrongFocus);
	m_volume = new Model(this);
	m_volume->load("./data/hand/hand.dat");

	alpha = 25;
	beta = -25;
	distance = 2.0;

	m_projectionMatrix.setToIdentity();
	m_rotateMatrix.setToIdentity();
	m_scaleMatrix.setToIdentity();
	m_translateMatrix.setToIdentity();
	m_translateMatrix.translate(0.0, 0.0, -1.0 * sqrt(3.0));
}


Renderer::~Renderer()
{
}


Model* Renderer::getVolume()
{
	return m_volume;
}

void Renderer::initializeGL()
{

	initializeOpenGLFunctions();

	//Cube::instance();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Set global information
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/raycast-vs.glsl");
	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/raycast-fs.glsl");
	shaderProgram.link();
	shaderProgram.bind();

	qDebug() << shaderProgram.isLinked();
	vertices << QVector2D(-1.0, -1.0) << QVector2D(1.0, -1.0) << QVector2D(1.0, 1.0) << QVector2D(-1.0, 1.0);
}


void Renderer::resizeGL(int width, int height)
{
	if (height == 0) {
		height = 1;
	}
	qreal aspectRatio = qreal(width) / qreal(height);

	qreal nearPlane = 0.5;
	qreal farPlane = 32.0;
	qreal fov = 60.0;

	m_projectionMatrix.setToIdentity();
	m_projectionMatrix.perspective(fov, aspectRatio, nearPlane, farPlane);
	
	//m_projectionMatrix.setToIdentity();
	//m_projectionMatrix.perspective(60.0, (float)width / (float)height, 0.001, 1000);

	glViewport(0, 0, width, height);
}

void Renderer::paintGL()
{
	// Clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);


	QMatrix4x4 mMatrix;
	QMatrix4x4 vMatrix;

	QMatrix4x4 cameraTransformation;
	//cameraTransformation.rotate(alpha, 0, 1, 0);
	//cameraTransformation.rotate(beta, 1, 0, 0);
	QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
	QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
	vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

	shaderProgram.bind();

	glActiveTexture(GL_TEXTURE0);
	m_volume->bind();

	shaderProgram.setUniformValue("zCoord", m_zCoord);
	shaderProgram.setUniformValue("volumeTexture", 0);
	shaderProgram.setUniformValue("modelViewProjectionMatrix", m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	shaderProgram.setUniformValue("inverseModelViewProjectionMatrix", (m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted());
	shaderProgram.setUniformValue("voxelSpacing", m_volume->getVoxelSpacing());
	shaderProgram.setUniformValue("dimensionScaling", m_volume->getDimensionScale());

	shaderProgram.setAttributeArray("vertex", vertices.constData());
	shaderProgram.enableAttributeArray("vertex");
	glDrawArrays(GL_QUADS, 0, vertices.size());
	shaderProgram.disableAttributeArray("vertex");

	glActiveTexture(GL_TEXTURE0);
	m_volume->release();
	
	shaderProgram.release();
}

void Renderer::mousePressEvent(QMouseEvent* event)
{
	m_currentX = qreal(event->x());
	m_currentY = qreal(event->y());

	m_previousX = m_currentX;
	m_previousY = m_currentY;
}

void Renderer::mouseMoveEvent(QMouseEvent* event)
{
	m_currentX = qreal(event->x());
	m_currentY = qreal(event->y());

	if (event->buttons() & Qt::LeftButton)
	{
		if (m_currentX != m_previousX || m_currentY != m_previousY)
		{
			m_translateMatrix.setColumn(3, m_translateMatrix.column(3) + QVector3D((m_previousX - m_currentX) * -0.001, (m_previousY - m_currentY) * 0.001, 0.0));
		}
	}

	m_previousX = m_currentX;
	m_previousY = m_currentY;

	update();
}

void Renderer::wheelEvent(QWheelEvent* event)
{
	float* values = new float[16];
	m_scaleMatrix.copyDataTo(values);
	QMatrix4x4 scaledMat = QMatrix4x4(values);
	delete[] values;

	float scaleFac = 1 + (float(event->delta()) / 120.0);
	scaleFac = scaleFac < 0 ? 0.0 : scaleFac;
	scaledMat.scale(scaleFac, scaleFac, scaleFac);

	float currScale = scaledMat.column(0).toVector3D().length();
	//qDebug() << currScale;

	if (currScale > 2.0 || currScale < 0.5)
	{
		qDebug() << "Out of scale";
		event->accept();
		return;
	}
	m_scaleMatrix.scale(scaleFac, scaleFac, scaleFac);
	update();

	event->accept();
}


void Renderer::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Left)
	{
		m_rotateMatrix.rotate(5.0, QVector3D(0.0,1.0,0.0));
		update();
	}
	else if (event->key() == Qt::Key_Right)
	{
		m_rotateMatrix.rotate(-5.0, QVector3D(0.0, 1.0, 0.0));
		update();
	}
	else if (event->key() == Qt::Key_Up)
	{
		m_rotateMatrix.rotate(5.0, QVector3D(1.0, 0.0, 0.0));
		update();
	}
	else if (event->key() == Qt::Key_Down)
	{
		m_rotateMatrix.rotate(-5.0, QVector3D(1.0, 0.0, 0.0));
		update();
	}
	else if (event->key() == Qt::Key_Comma)
	{
		m_rotateMatrix.rotate(5.0, QVector3D(0.0, 0.0, 1.0));
		update();
	}
	else if (event->key() == Qt::Key_Period)
	{
		m_rotateMatrix.rotate(-5.0, QVector3D(0.0, 0.0, 1.0));
		update();
	}
	event->accept();
}