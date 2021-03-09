
#include "Renderer.h"

Renderer::Renderer()
{
	//openglWidget = opengl;
	m_model = std::make_unique<Model>(Model("data/wholebody/wholebody.dat"));

	alpha = 25;
	beta = -25;
	distance = 2.5;

}

bool Renderer::myFunc(QWidget* f)
{
	return true;
}

Renderer::~Renderer()
{
}



void Renderer::initializeGL()
{

	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// Set global information
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/vertexShader.glsl");
	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/fragmentShader.glsl");
	shaderProgram.link();
	shaderProgram.bind();

	createTexture();

	vertices << QVector3D(-1.0, -1.0, 0.0) << QVector3D(1.0, -1.0, 0.0) << QVector3D(1.0, 1.0, 0.0) << QVector3D(-1.0, 1.0, 0.0);
}

void Renderer::createTexture()
{
	glDeleteTextures(1, &m_textureID);

	auto dimensions = m_model->getDimensions();
	glGenTextures(1, &m_textureID);

	std::vector<unsigned short> buffer;

	unsigned int y = m_div;

	for (unsigned int z = 0; z < dimensions.at(2); z++)
	{
		for (unsigned int x = 0; x < dimensions.at(0); x++)
		{
			buffer.push_back(m_model->getValue(x, y, z) * 16);
			buffer.push_back(0);
			buffer.push_back(0);
			buffer.push_back(0);
		}
	}

	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.at(0), dimensions.at(2), 0, GL_RGBA, GL_UNSIGNED_SHORT, buffer.data());
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Renderer::resizeGL(int width, int height)
{
	if (height == 0) {
		height = 1;
	}
	pMatrix.setToIdentity();
	pMatrix.perspective(60.0, (float)width / (float)height, 0.001, 1000);
	glViewport(0, 0, width, height);
}

void Renderer::paintGL()
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	if (m_div != m_prev)
	{
		m_prev = m_div;
		createTexture();
	}

	QMatrix4x4 mMatrix;
	QMatrix4x4 vMatrix;

	QMatrix4x4 cameraTransformation;
	cameraTransformation.rotate(alpha, 0, 1, 0);
	cameraTransformation.rotate(beta, 1, 0, 0);
	QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
	QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
	vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

	shaderProgram.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	

	shaderProgram.setUniformValue("texture1", 0);
	shaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);
	shaderProgram.setUniformValue("color", QColor(Qt::white));
	shaderProgram.setAttributeArray("vertex", vertices.constData());
	shaderProgram.enableAttributeArray("vertex");
	glDrawArrays(GL_QUADS, 0, vertices.size());
	shaderProgram.disableAttributeArray("vertex");
	glBindTexture(GL_TEXTURE_2D, 0);
	shaderProgram.release();

	
}


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
}
void Renderer::wheelEvent(QWheelEvent* event)
{
	int delta = event->delta();
	if (event->orientation() == Qt::Vertical) {
		if (delta < 0) {
			distance *= 1.1;
		}
		else if (delta > 0) {
			distance *= 0.9;
		}
		update();
	}
	event->accept();
}
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
