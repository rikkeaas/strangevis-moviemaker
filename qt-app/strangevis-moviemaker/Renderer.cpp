
#include "Renderer.h"
#include "cube.h"
#include "strangevismoviemaker.h"
#include <QtMath>
#include <QPixmap>
#include <QMessageBox>

Renderer::Renderer() : QOpenGLWidget(nullptr, Qt::WindowFlags())
{}

Renderer::Renderer(QWidget* parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f)
{
	setFocusPolicy(Qt::StrongFocus);
	m_volume = new Model(this);
	m_volume->load("./data/hand/hand.dat");

	m_keyframeHandler = new KeyframeHandler();
	QObject::connect(m_keyframeHandler, &KeyframeHandler::matricesUpdated, this, &Renderer::setMatrices);
	QObject::connect(m_keyframeHandler, &KeyframeHandler::addedKeyframe, this, &Renderer::addNewKeyframe);
	QObject::connect(m_keyframeHandler, &KeyframeHandler::deletedKeyframe, this, &Renderer::updateKeyframes);

	interpolater = new LinearInterpolation();

	m_phasefunction = new PhaseFunction();

	alpha = 25;
	beta = -25;
	distance = 2.0;

	m_projectionMatrix.setToIdentity();
	m_rotateMatrix.setToIdentity();
	m_scaleMatrix.setToIdentity();
	m_translateMatrix.setToIdentity();
	m_translateMatrix.translate(0.0, 0.0, -2.0 * sqrt(3.0));

	m_backgroundColor = QVector3D(0.0, 0.0, 0.0);
	m_phaseFunctionData = m_phasefunction->getPhaseFunctionData();
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

void Renderer::setState()
{
	QList<float*> mx;
	mx.append(m_projectionMatrix.data());
	mx.append(m_rotateMatrix.data());
	mx.append(m_scaleMatrix.data());
	mx.append(m_translateMatrix.data());
	qDebug() << "Background color in renderer: " << m_backgroundColor;
	m_phaseFunctionData = m_phasefunction->getPhaseFunctionData();
	m_keyframeHandler->saveState(this, m_volume->getFilename(), mx, m_backgroundColor, m_phasefunction->getPhaseFunctionData());
	m_keyframeHandler->takeQtScreenShot(this, m_volume->getFilename());
}

void Renderer::addNewKeyframe()
{
	setState();
	setKeyframes(keyframeWrapper, square);
}

void Renderer::updateKeyframes()
{
	setKeyframes(keyframeWrapper, square);
}

QWidget* Renderer::setKeyframes(QWidget* keyframeWrapper, QSize* sq) {
	square = sq;
	keyframeWrapper = m_keyframeHandler->updateKeyframes(keyframeWrapper, square, m_volume->getFilename());
	keyframeWrapper->setFixedSize(*square);
	keyframeWrapper->setStyleSheet("background-color: #3C3C3C;");
	return keyframeWrapper;
}

void Renderer::setKeyframeWrapper(QWidget* qw) {
	keyframeWrapper = qw;
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
	glActiveTexture(GL_TEXTURE0+1);
	m_phasefunction->bind();

	shaderProgram.setUniformValue("zCoord", m_zCoord);
	shaderProgram.setUniformValue("volumeTexture", 0);
	shaderProgram.setUniformValue("phaseFunction", 1);
	shaderProgram.setUniformValue("lightPosition", QVector3D((m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted() * QVector4D(5.0, 0.0, 0.0, 1.0)));
	shaderProgram.setUniformValue("modelViewMatrix", m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	shaderProgram.setUniformValue("modelViewProjectionMatrix", m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	//qDebug() << "mvp " << m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
	shaderProgram.setUniformValue("inverseModelViewProjectionMatrix", (m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted());
	shaderProgram.setUniformValue("voxelSpacing", m_volume->getVoxelSpacing());
	shaderProgram.setUniformValue("dimensionScaling", m_volume->getDimensionScale());
	shaderProgram.setUniformValue("voxelDimsInTexCoord", QVector3D(QVector3D(1.0,1.0,1.0) / m_volume->getDimensions()));
	shaderProgram.setUniformValue("backgroundColorVector", m_backgroundColor);

	shaderProgram.setAttributeArray("vertex", vertices.constData());
	shaderProgram.enableAttributeArray("vertex");
	glDrawArrays(GL_QUADS, 0, vertices.size());
	shaderProgram.disableAttributeArray("vertex");

	glActiveTexture(GL_TEXTURE0+1);
	m_phasefunction->release();
	glActiveTexture(GL_TEXTURE0);
	m_volume->release();
	
	
	shaderProgram.release();
	float elapsedTime = timer.restart()/animationDuration;
	t += elapsedTime;
	if (t <= 1) {
		QList<QMatrix4x4> newMatrices = interpolater->interpolate(fromKeyframe, toKeyframe, t);
		m_projectionMatrix = newMatrices[0];
		m_rotateMatrix = newMatrices[1];
		m_scaleMatrix = newMatrices[2];
		m_translateMatrix = newMatrices[3];
		m_backgroundColor = interpolater->backgroundInterpolation(fromBackgroundColor, toBackgroundColor, t);
		m_phaseFunctionData = interpolater->phaseFunctionInterpolation(fromPhaseFunction, toPhaseFunction, t);
		m_phasefunction->updatePhaseFunction(0, 256, &m_phaseFunctionData);
		update();
	}
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
	float scaleFac = 1 + (float(event->delta()) / 1200.0);
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
		m_keyframeHandler->toDelete = false;
	} 
	else if (event->key() == Qt::Key_K) {
		addNewKeyframe();
	}
	else if (event->key() == Qt::Key_C) {
		bool ok;
		QMessageBox msgBox;
		msgBox.setWindowTitle("Clearing states");
		msgBox.setText("Do you want to clear all states?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes) {
			clearStates();
		}
	}
	else if (event->key() == Qt::Key_B) {
		setBackgroundColor();
	}
	else if (event->key() == Qt::Key_A) {
		playAnimation();
	}
	else if (event->key() == Qt::Key_P) {
		qDebug() << m_phaseFunctionData;
	}
}




void Renderer::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Shift)
	{
		m_rotating = true;
		m_keyframeHandler->toDelete = true;
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

void Renderer::clearStates()
{
	QDirIterator it("./states/", { "*.png" });

	while (it.hasNext())
		QFile(it.next()).remove();

	QDirIterator at("./states/", { "*.txt" });

	while (at.hasNext())
		QFile(at.next()).remove();
	setKeyframes(keyframeWrapper, square);
}

void Renderer::setMatrices(QList<QMatrix4x4> matrices, QVector3D backgroundColor, QVector<float> phaseFunction) {
	t = 0;
	timer.restart();
	fromKeyframe = QList<QMatrix4x4>({ m_projectionMatrix, m_rotateMatrix, m_scaleMatrix, m_translateMatrix });
	toKeyframe = matrices;
	fromBackgroundColor = m_backgroundColor;
	toBackgroundColor = backgroundColor;
	fromPhaseFunction = m_phaseFunctionData;
	toPhaseFunction = phaseFunction;
	update();
}

void Renderer::setBackgroundColor()
{
	auto colorpicker = QColorDialog::getColor();
	m_backgroundColor.setX(colorpicker.red() / 255.f);
	m_backgroundColor.setY(colorpicker.green() / 255.f);
	m_backgroundColor.setZ(colorpicker.blue() / 255.f);
	update();
}

void Renderer::playAnimation()
{
	auto states = m_keyframeHandler->getFiles();
	if (states.at(1).length() > 0) {
		auto backupMatrices = QList<QMatrix4x4>({ m_projectionMatrix, m_rotateMatrix, m_scaleMatrix, m_translateMatrix });
		auto backupBackgroundColor = m_backgroundColor;
		auto backupPhaseFunction = m_phaseFunctionData;
		int index = 0;
		int numberOfStates = states.at(1).length();
		int keyframeHighlightIndex = numberOfStates - 1;
		while (true) {
			if (index >= numberOfStates) {
				break;
			}
			m_keyframeHandler->readStates("states/" + states.at(1).at(index));
			if (keyframeHighlightIndex < 9) {
				m_keyframeHandler->highlightKeyframe(keyframeWrapper, keyframeHighlightIndex);
			}
			QEventLoop loop;
			QTimer::singleShot(animationDuration, &loop, SLOT(quit()));
			loop.exec();
			if (keyframeHighlightIndex < 9) {
				m_keyframeHandler->removeKeyframeHighlighting(keyframeWrapper, keyframeHighlightIndex);
			}
			index++;
			keyframeHighlightIndex--;
		}
		setMatrices(backupMatrices, backupBackgroundColor, backupPhaseFunction);
	}
	else {
		qDebug() << "Can't play animation with no saved states.";
	}
	
}


PhaseFunction* Renderer::getPhaseFunction()
{
	return m_phasefunction;
}

int Renderer::getAnimationDuration()
{
	return animationDuration / 1000;
}

void Renderer::setAnimationDuration(double newDur)
{
	animationDuration = newDur * 1000.f;
}