
#include "Renderer.h"
#include "cube.h"
#include <QtMath>

Renderer::Renderer(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f)
{
	m_volume = new Model(this);

	alpha = 25;
	beta = -25;
	distance = 2.0;

	//m_projectionMatrix.setToIdentity();
	m_modelViewMatrix.setToIdentity();
	m_modelViewMatrix.translate(0.0, 0.0, -2.0 * sqrt(3.0));
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

	Cube::instance();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Set global information
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/vertexShader.glsl");
	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/fragmentShader.glsl");
	shaderProgram.link();
	shaderProgram.bind();

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
}

void Renderer::paintGL()
{
	// Clear
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


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
	shaderProgram.setUniformValue("texture1", 0);
	shaderProgram.setUniformValue("mvpMatrix", m_projectionMatrix * m_modelViewMatrix);
	shaderProgram.setUniformValue("color", QColor(Qt::white));

	Cube::instance()->bindCube();

	int location = shaderProgram.attributeLocation("vertexPosition");
	shaderProgram.enableAttributeArray(location);
	shaderProgram.setAttributeBuffer(location, GL_FLOAT, 0, 3, sizeof(QVector3D));

	Cube::instance()->drawCube();
	//glDrawArrays(GL_QUADS, 0, vertices.size());

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
			QVector3D va = arcballVector(m_previousX, m_previousY);
			QVector3D vb = arcballVector(m_currentX, m_currentY);

			if (va != vb)
			{
				qreal angle = acos(qMax(-1.0f, qMin(1.0f, QVector3D::dotProduct(va, vb))));
				QVector3D axis = QVector3D::crossProduct(va, vb);

				QMatrix4x4 inverseModelViewMatrix = m_modelViewMatrix.inverted();
				QVector4D transformedAxis = inverseModelViewMatrix * QVector4D(axis, 0.0f);

				m_modelViewMatrix.rotate(qRadiansToDegrees(angle), transformedAxis.toVector3D());
			}
		}


	}

	m_previousX = m_currentX;
	m_previousY = m_currentY;

	update();
}

QVector3D Renderer::arcballVector(qreal x, qreal y)
{
	QVector3D p = QVector3D(2.0f * float(x) / float(width()) - 1.0f, -2.0f * float(y) / float(height()) + 1.0f, 0.0);

	float length2 = p.x() * p.x() + p.y() * p.y();

	if (length2 < 1.0f)
		p.setZ(sqrtf(1.0f - length2));
	else
		p.normalize();

	return p;
}

/*
void Renderer::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::RightButton) 
	{
		m_prev = m_div;
		m_div += 1;
		m_div = m_div >= m_model->getDimensions().at(1) ? m_model->getDimensions().at(1) - 1 : m_div;
	}
	else
	{
		m_prev = m_div;
		m_div -= 1;
		m_div = m_div < 0 ? 0 : m_div;
	}
	lastMousePosition = event->pos();
	event->accept();

	update();
}*/
void Renderer::wheelEvent(QWheelEvent* event)
{
	int delta = event->delta();
	if (event->orientation() == Qt::Vertical) {
		if (delta > 0) {
			m_zCoord = 1.0 < m_zCoord + float(delta) / 120.0 ? 1.0 : m_zCoord + float(delta) / 120.0;
		}
		else if (delta < 0) {
			m_zCoord = 0.0 > m_zCoord + float(delta) / 120.0 ? 0.0 : m_zCoord + float(delta) / 120.0;
		}
		update();
	}
	event->accept();
}

/*
void Renderer::mouseMoveEvent(QMouseEvent* event)
{
	int deltaX = event->x() - lastMousePosition.x();
	int deltaY = event->y() - lastMousePosition.y();
	if (event->buttons() & Qt::LeftButton) {
		alpha -= deltaX;
		while (alpha < 0) {
			alpha += 360;
		}
		while (alpha >= 360) {
			alpha -= 360;
		}
		beta -= deltaY;
		if (beta < -90) {
			beta = -90;
		}
		if (beta > 90) {
			beta = 90;
		}
		update();
	}
	lastMousePosition = event->pos();
	event->accept();
}
*/
