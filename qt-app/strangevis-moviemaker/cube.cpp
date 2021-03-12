#include "cube.h"
#include <QVector3D>
#include <QDebug>

Cube::Cube() : m_quadVertexBuffer(QOpenGLBuffer::VertexBuffer), m_quadIndexBuffer(QOpenGLBuffer::IndexBuffer), m_cubeVertexBuffer(QOpenGLBuffer::VertexBuffer), m_cubeIndexBuffer(QOpenGLBuffer::IndexBuffer)
{
    QVector3D verticesQuad[] = {
      QVector3D(-1.0f, -1.0f, 0.0f), // bottom left corner
      QVector3D(-1.0f,  1.0f, 0.0f), // top left corner
      QVector3D(1.0f,  1.0f, 0.0f), // top right corner
      QVector3D(1.0f, -1.0f, 0.0f)  // bottom right corner
    };

    GLushort indicesQuad[] = {
      0,1,2, // first triangle (bottom left - top left - top right)
      0,2,3, // second triangle (bottom left - top right - bottom right)
    };

    m_quadVertexBuffer.create();
    m_quadVertexBuffer.bind();
    m_quadVertexBuffer.allocate(verticesQuad, sizeof(verticesQuad));

    m_quadIndexBuffer.create();
    m_quadIndexBuffer.bind();
    m_quadIndexBuffer.allocate(indicesQuad, sizeof(indicesQuad));

    QVector3D verticesCube[] = {
        // front
        QVector3D(-1.0f, -1.0f,  1.0f),
        QVector3D(1.0f, -1.0f,  1.0f),
        QVector3D(1.0f,  1.0f,  1.0f),
        QVector3D(-1.0f,  1.0f,  1.0f),

        // back
        QVector3D(-1.0f, -1.0f,  -1.0f),
        QVector3D(1.0f, -1.0f,  -1.0f),
        QVector3D(1.0f,  1.0f,  -1.0f),
        QVector3D(-1.0f,  1.0f,  -1.0f),
    };

    GLushort indicesCube[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    m_cubeVertexBuffer.create();
    m_cubeVertexBuffer.bind();
    m_cubeVertexBuffer.allocate(verticesCube, sizeof(verticesCube));

    m_cubeIndexBuffer.create();
    m_cubeIndexBuffer.bind();
    m_cubeIndexBuffer.allocate(indicesCube, sizeof(indicesCube));

}

void Cube::bindQuad()
{
    m_quadVertexBuffer.bind();
    m_quadIndexBuffer.bind();
}

void Cube::drawQuad()
{
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Cube::bindCube()
{
    m_cubeVertexBuffer.bind();
    m_cubeIndexBuffer.bind();
}

void Cube::drawCube()
{
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}

Cube* Cube::instance()
{
    static Cube* geo = nullptr;

    if (geo == nullptr)
        geo = new Cube();

    return geo;
}

