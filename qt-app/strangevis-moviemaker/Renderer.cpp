
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
	m_volume->threadedLoading("./data/hand/hand.dat");
	connect(m_volume, &Model::loadedModel, this, &Renderer::updateWidget);

	m_border = new testWidget(this);
	m_border->setHidden(true);

	m_keyframeHandler = new KeyframeHandler();
	QObject::connect(m_keyframeHandler, &KeyframeHandler::matricesUpdated, this, &Renderer::setMatrices);
	QObject::connect(m_keyframeHandler, &KeyframeHandler::addedKeyframe, this, &Renderer::addNewKeyframe);
	QObject::connect(m_keyframeHandler, &KeyframeHandler::deletedKeyframe, this, &Renderer::updateKeyframes);

	interpolater = new LinearInterpolation();

	m_transferfunction = new TransferFunction();

	alpha = 25;
	beta = -25;
	distance = 2.0;

	m_projectionMatrix.setToIdentity();
	m_rotateMatrix.setToIdentity();
	m_scaleMatrix.setToIdentity();
	m_translateMatrix.setToIdentity();
	m_translateMatrix.translate(0.0, 0.0, -2.0 * sqrt(3.0));

	m_lightRotateMatrix.setToIdentity();
	m_lightTranslateMatrix.setToIdentity();

	m_backgroundColor = QVector3D(0.0, 0.0, 0.0);
	m_transferFunctionData = m_transferfunction->getTransferFunctionData();
}


Renderer::~Renderer()
{
	m_volume->~Model();
	m_transferfunction->~TransferFunction();
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
	m_transferFunctionData = m_transferfunction->getTransferFunctionData();
	m_keyframeHandler->saveState(this, m_volume->getFilename(), mx, m_backgroundColor, m_transferfunction->getTransferFunctionData(), m_layers);
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
	m_transferfunction->bind();

	shaderProgram.setUniformValue("zCoord", m_zCoord);
	shaderProgram.setUniformValue("volumeTexture", 0);
	shaderProgram.setUniformValue("transferFunction", 1);
	shaderProgram.setUniformValue("lightPosition", QVector3D((m_lightTranslateMatrix * m_lightRotateMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted() * QVector4D(5.0, 0.0, -5.0, 1.0)));
	shaderProgram.setUniformValue("modelViewMatrix", m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	shaderProgram.setUniformValue("rotate", m_rotateMatrix);
	shaderProgram.setUniformValue("invRotate", m_rotateMatrix.inverted());
	shaderProgram.setUniformValue("sphereCut", m_sphereCut);
	shaderProgram.setUniformValue("cubeCut", m_cubeCut);
	shaderProgram.setUniformValue("sphereCutRadius", float(m_sphereCutRadius));
	shaderProgram.setUniformValue("cubeCutSize", float(m_cubeCutSize));
	shaderProgram.setUniformValue("showCut", m_showCut);
	shaderProgram.setUniformValue("showInFront", m_showInFront);

	shaderProgram.setUniformValue("inverseModelViewMatrix", (m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted());
	shaderProgram.setUniformValue("modelViewProjectionMatrix", m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix);
	//qDebug() << "mvp " << m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix;
	shaderProgram.setUniformValue("inverseModelViewProjectionMatrix", (m_projectionMatrix * m_translateMatrix * m_rotateMatrix * m_scaleMatrix).inverted());
	shaderProgram.setUniformValue("voxelSpacing", m_volume->getVoxelSpacing());
	shaderProgram.setUniformValue("dimensionScaling", m_volume->getDimensionScale());
	shaderProgram.setUniformValue("voxelDimsInTexCoord", QVector3D(QVector3D(1.0,1.0,1.0) / m_volume->getDimensions()));
	shaderProgram.setUniformValue("backgroundColorVector", m_backgroundColor);
	shaderProgram.setUniformValue("samplingDistanceMultiplier", m_raySamplingDistanceMultiplier);
	shaderProgram.setUniformValue("skippingStep", m_skippingStep);

	shaderProgram.setAttributeArray("vertex", vertices.constData());
	shaderProgram.enableAttributeArray("vertex");
	glDrawArrays(GL_QUADS, 0, vertices.size());
	shaderProgram.disableAttributeArray("vertex");

	glActiveTexture(GL_TEXTURE0+1);
	m_transferfunction->release();
	glActiveTexture(GL_TEXTURE0);
	m_volume->release();
	
	
	shaderProgram.release();
	float elapsedTime = timer.restart()/animationDuration;
	t += elapsedTime;
	if (t < 1) {
		isInterpolating = true;

		if (catmullRom)
		{
			QList<QMatrix4x4> newMatrices = CatmullRomInterpolation::matrixInterpolation(previousKeyframe, fromKeyframe, toKeyframe, nextKeyframe, t);
			m_projectionMatrix = newMatrices[0];
			m_rotateMatrix = newMatrices[1];
			m_scaleMatrix = newMatrices[2];
			m_translateMatrix = newMatrices[3];
			m_backgroundColor = CatmullRomInterpolation::vectorInterpolation(previousBackgroundColor, fromBackgroundColor, toBackgroundColor, nextBackgroundColor, t);
			m_transferFunctionData = CatmullRomInterpolation::transferFunctionInterpolation(previousTransferFunction, fromTransferFunction, toTransferFunction, nextTransferFunction, t);
			m_transferfunction->updateTransferFunction(0, 512, &m_transferFunctionData);
		}
		else
		{
			QList<QMatrix4x4> newMatrices = interpolater->interpolate(fromKeyframe, toKeyframe, t);
			m_projectionMatrix = newMatrices[0];
			m_rotateMatrix = newMatrices[1];
			m_scaleMatrix = newMatrices[2];
			m_translateMatrix = newMatrices[3];
			m_backgroundColor = interpolater->backgroundInterpolation(fromBackgroundColor, toBackgroundColor, t);
			m_transferFunctionData = interpolater->transferFunctionInterpolation(fromTransferFunction, toTransferFunction, t);
			m_transferfunction->updateTransferFunction(0, 512, &m_transferFunctionData);
		}
		update();
	}
	if (t >= 1 && isInterpolating) {
		m_projectionMatrix = toKeyframe[0];
		m_rotateMatrix = toKeyframe[1];
		m_scaleMatrix = toKeyframe[2];
		m_translateMatrix = toKeyframe[3];
		m_backgroundColor = toBackgroundColor;
		m_transferFunctionData = toTransferFunction;
		m_transferfunction->updateTransferFunction(0, 512, &toTransferFunction);
		isInterpolating = false;
		updateLayers(m_layers);
		update();
	}
}

void Renderer::mousePressEvent(QMouseEvent* event)
{
	m_currentX = qreal(event->x());
	m_currentY = qreal(event->y());

	m_previousX = m_currentX;
	m_previousY = m_currentY;

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
					
					if (m_transformLight)
					{
						QVector3D axis = QVector3D::crossProduct(vb, va);
						QMatrix4x4 inverseModelViewMatrix = m_lightRotateMatrix.inverted();
						QVector4D transformedAxis = inverseModelViewMatrix * QVector4D(axis, 0.0f);

						m_lightRotateMatrix.rotate(qRadiansToDegrees(angle), transformedAxis.toVector3D());
					}
					else
					{
						QVector3D axis = QVector3D::crossProduct(va, vb);
						QMatrix4x4 inverseModelViewMatrix = m_rotateMatrix.inverted();
						QVector4D transformedAxis = inverseModelViewMatrix * QVector4D(axis, 0.0f);

						m_rotateMatrix.rotate(qRadiansToDegrees(angle), transformedAxis.toVector3D());
					}
				}
			}
			else
			{
				if (m_transformLight)
				{
					m_lightTranslateMatrix.setColumn(3, m_lightTranslateMatrix.column(3) + QVector3D((m_previousX - m_currentX) * -0.001, (m_previousY - m_currentY) * 0.001, 0.0));
				}
				else
				{
					m_translateMatrix.setColumn(3, m_translateMatrix.column(3) + QVector3D((m_previousX - m_currentX) * -0.001, (m_previousY - m_currentY) * 0.001, 0.0));
				}
			}
		}
	}

	m_previousX = m_currentX;
	m_previousY = m_currentY;

	update();
}

void Renderer::wheelEvent(QWheelEvent* event)
{
	if (m_transformLight)
	{
		event->accept();
		return;
	}

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
		//qDebug() << "Out of scale";
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
		if (!animationIsPlaying) {
			playAnimation();
		}
	}
	else if (event->key() == Qt::Key_P) {
		qDebug() << m_transferFunctionData;
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

void Renderer::setMatrices(QList<QMatrix4x4> matrices, QVector3D backgroundColor, QVector<float> transferFunction, QList<Layer*> layers) {
	t = 0;
	timer.restart();
	fromKeyframe = QList<QMatrix4x4>({ m_projectionMatrix, m_rotateMatrix, m_scaleMatrix, m_translateMatrix });
	toKeyframe = matrices;
	fromBackgroundColor = m_backgroundColor;
	toBackgroundColor = backgroundColor;
	fromTransferFunction = m_transferFunctionData;
	toTransferFunction = transferFunction;
    setLayers(layers);
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
	animationIsPlaying = true;
	auto states = m_keyframeHandler->getFiles();
	if (states.at(1).length() > 0) {
		auto backupMatrices = QList<QMatrix4x4>({ m_projectionMatrix, m_rotateMatrix, m_scaleMatrix, m_translateMatrix });
		auto backupBackgroundColor = m_backgroundColor;
		auto backuptransferFunction = m_transferfunction->getTransferFunctionData();
		QList<Layer*> backupLayers;
		for (Layer* l : m_layers)
		{
			Layer* nl = new Layer(nullptr, l->m_selectedArea, true, l->m_layerRGBA);
			nl->label->setText(l->label->text());
			nl->m_selectedArea = l->m_selectedArea;
			nl->m_layerRGBA = l->m_layerRGBA;
			nl->setStyleSheet("background-color:#6D6D6D; height:45; border-radius:10px;");
			backupLayers.append(nl);
		}

		int index = 0;
		int numberOfStates = states.at(1).length();
		int keyframeHighlightIndex = numberOfStates - 1;

		QList<QList<QMatrix4x4>> matrices;
		QList<QVector3D> backgrounds;
		QList<QVector<float>> transferFunctions;
		QList<QList<Layer*>> layers;

		if (interpolationTypeIsCM)
		{
			for (int i = 0; i < numberOfStates; i++)
			{
				QList<QMatrix4x4> mats = QList<QMatrix4x4>();
				QVector3D background = QVector3D();
				QVector<float> tf = QVector<float>();
				QList<Layer*> ls = QList<Layer*>();

				m_keyframeHandler->getStates("states/" + states.at(1).at(i), mats, background, tf, ls);

				matrices.append(mats);
				backgrounds.append(background);
				transferFunctions.append(tf);
				layers.append(ls);
			}
		}


		while (true) {
			if (index >= numberOfStates) {
				break;
			}
			if (!interpolationTypeIsCM)
			{
				m_keyframeHandler->readStates("states/" + states.at(1).at(index));
			}
			else
			{
				fromKeyframe = QList<QMatrix4x4>({ m_projectionMatrix, m_rotateMatrix, m_scaleMatrix, m_translateMatrix });
				fromBackgroundColor = m_backgroundColor;
				fromTransferFunction = m_transferFunctionData;
				m_layers = layers[index];
				updateLayers(m_layers);
				if (index == 0)
				{
					previousKeyframe = fromKeyframe;
					previousBackgroundColor = fromBackgroundColor;
					previousTransferFunction = fromTransferFunction;
				}
				else
				{
					previousKeyframe = matrices[index - 1];
					previousBackgroundColor = backgrounds[index - 1];
					previousTransferFunction = transferFunctions[index - 1];
				}
				toKeyframe = matrices[index];
				toBackgroundColor = backgrounds[index];
				toTransferFunction = transferFunctions[index];
				if (index == numberOfStates - 1)
				{
					nextKeyframe = backupMatrices;
					nextBackgroundColor = backupBackgroundColor;
					nextTransferFunction = backuptransferFunction;
				}
				else
				{
					nextKeyframe = matrices[index + 1];
					nextBackgroundColor = backgrounds[index + 1];
					nextTransferFunction = transferFunctions[index + 1];
				}

				t = 0.0;
				catmullRom = true;
				timer.restart();
				update();
			}

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
		setMatrices(backupMatrices, backupBackgroundColor, backuptransferFunction, backupLayers);
		catmullRom = false;
	}
	else {
		qDebug() << "Can't play animation with no saved states.";
	}
	animationIsPlaying = false;
}

void Renderer::setInterpolationType(bool b)
{
	interpolationTypeIsCM = b;
}

TransferFunction* Renderer::getTransferFunction()
{
	return m_transferfunction;
}

int Renderer::getAnimationDuration()
{
	return animationDuration / 1000;
}

void Renderer::setAnimationDuration(double newDur)
{
	animationDuration = newDur * 1000.f;
}

void Renderer::setLayers(QList<Layer*> layers)
{
	m_layers = layers;
}

void Renderer::setSphereCut(bool cut)
{
	m_sphereCut = cut;
}
void Renderer::setCubeCut(bool cut)
{
	m_cubeCut = cut;
}

void Renderer::setSphereRadius(double rad)
{
	m_sphereCutRadius = rad;
}

void Renderer::setCubeSize(double size)
{
	m_cubeCutSize = size;
}

void Renderer::setShowCut(bool show, bool inFront)
{
	m_showCut = show;
	m_showInFront = inFront;
}

void Renderer::toggleLightVolumeTransformation()
{
	m_transformLight = !m_transformLight;
	if (m_transformLight)
	{
		m_border->setHidden(false);
	}
	else
	{
		m_border->setHidden(true);
	}
}

void Renderer::updateWidget()
{
	reloadDockWidgets();
	update();
}
void Renderer::setRaySamplingDistance(float newSamplingDistance)
{
	m_raySamplingDistanceMultiplier = newSamplingDistance;
}

float Renderer::getRaySamplingDistance()
{
	return m_raySamplingDistanceMultiplier;
}


int Renderer::getSkippingStep()
{
	return m_skippingStep;
}

void Renderer::setSkippingStep(int step)
{
	m_skippingStep = step;
}


testWidget::testWidget(QWidget* parent) : QWidget(parent)
{
	auto text = new QLineEdit();
	QHBoxLayout* layout = new QHBoxLayout();
	text->setText("Light mode");
	int id = QFontDatabase::addApplicationFont("fonts/Roboto-Bold.ttf");
	QString robotoHeader = QFontDatabase::applicationFontFamilies(id).at(0);
	QFont f(robotoHeader, 10);
	text->setFont(f);
	text->setTextMargins(5, 5, 5, 5);
	text->setStyleSheet("QLineEdit {background-color: #4C4C4C; border: 5px solid white}");
	text->setReadOnly(true);
	text->setAlignment(Qt::AlignCenter);
	layout->addWidget(text);

	//layout->setMargin(5);
	layout->setContentsMargins(10, 10, 10, 10);

	setLayout(layout);
}

void testWidget::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
