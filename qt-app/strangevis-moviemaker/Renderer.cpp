
#include "Renderer.h"
#include "cube.h"
#include "strangevismoviemaker.h"
#include <QtMath>

Renderer::Renderer() : QOpenGLWidget(nullptr, Qt::WindowFlags())
{}

Renderer::Renderer(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f)
{
	setFocusPolicy(Qt::StrongFocus);
	m_volume = new Model(this);
	m_volume->load("./data/hand/hand.dat");

	m_phasefunction = new PhaseFunction();

	alpha = 25;
	beta = -25;
	distance = 2.0;

	m_projectionMatrix.setToIdentity();
	m_rotateMatrix.setToIdentity();
	m_scaleMatrix.setToIdentity();
	m_translateMatrix.setToIdentity();
	m_translateMatrix.translate(0.0, 0.0, -2.0 * sqrt(3.0));


}


Renderer::~Renderer()
{
	m_volume->~Model();
	m_phasefunction->~PhaseFunction();
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

	timer.start();
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
	nbFrames++;
	if (timer.elapsed() >= 1000.0) { // If last prinf() was more than 1 sec ago
		// printf and reset timer
		qDebug() << ("%f fps\n", nbFrames / ((double)timer.elapsed() / 1000.0));
		nbFrames = 0;
		timer.restart();
	}


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
	glActiveTexture(GL_TEXTURE0+1);
	m_phasefunction->bind();

	shaderProgram.setUniformValue("zCoord", m_zCoord);
	shaderProgram.setUniformValue("volumeTexture", 0);
	shaderProgram.setUniformValue("phaseFunction", 1);
	shaderProgram.setUniformValue("modelViewProjectionMatrix", m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	//qDebug() << "mvp " << m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
	shaderProgram.setUniformValue("inverseModelViewProjectionMatrix", (m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted());
	shaderProgram.setUniformValue("voxelSpacing", m_volume->getVoxelSpacing());
	shaderProgram.setUniformValue("dimensionScaling", m_volume->getDimensionScale());
	shaderProgram.setUniformValue("voxelDimsInTexCoord", QVector3D(QVector3D(1.0,1.0,1.0) / m_volume->getDimensions()));

	shaderProgram.setAttributeArray("vertex", vertices.constData());
	shaderProgram.enableAttributeArray("vertex");
	glDrawArrays(GL_QUADS, 0, vertices.size());
	shaderProgram.disableAttributeArray("vertex");

	glActiveTexture(GL_TEXTURE0+1);
	m_phasefunction->release();
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

	/*
	QVector<float> data;
	for (int i = 0; i < 256; i++)
	{
		if (clicks % 3 == 0)
			data << 1.0 << 0.0 << 0.0 << 1.0;
		else if (clicks % 3 == 1)
			data << 0.0 << 1.0 << 0.0 << 1.0;
		else 
			data << 0.0 << 0.0 << 1.0 << 1.0;
	}
	m_phasefunction->updatePhaseFunction(0, 256, &data);
	*/
	clicks++;
}

void Renderer::mouseMoveEvent(QMouseEvent* event)
{
	m_currentX = qreal(event->x());
	m_currentY = qreal(event->y());

	if (event->buttons() & Qt::LeftButton)
	{
		if (m_currentX != m_previousX || m_currentY != m_previousY)
		{
			if (m_rotating)
			{
				QVector3D va = arcballVector(m_previousX, m_previousY);
				QVector3D vb = arcballVector(m_currentX, m_currentY);

				if (va != vb)
				{
					qreal angle = acos(qMax(-1.0f, qMin(1.0f, QVector3D::dotProduct(va, vb))));
					QVector3D axis = QVector3D::crossProduct(va, vb);

					QMatrix4x4 inverseModelViewMatrix = m_rotateMatrix.inverted();
					QVector4D transformedAxis = inverseModelViewMatrix * QVector4D(axis, 0.0f);

					m_rotateMatrix.rotate(qRadiansToDegrees(angle), transformedAxis.toVector3D());
				}
			}
			else
			{
				m_translateMatrix.setColumn(3, m_translateMatrix.column(3) + QVector3D((m_previousX - m_currentX) * -0.001, (m_previousY - m_currentY) * 0.001, 0.0));
			}
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

void Renderer::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Shift)
	{
		m_rotating = false;
	}
}


void Renderer::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Shift)
	{
		m_rotating = true;
	}
	event->accept();
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


PhaseFunction* Renderer::getPhaseFunction()
{
	return m_phasefunction;
}