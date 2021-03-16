#pragma once
#include <QOpenGLBuffer>

class Cube
{
protected:
    Cube();

public:
    static Cube* instance();

    void bindQuad();
    void drawQuad();

    void bindCube();
    void drawCube();

private:
    QOpenGLBuffer m_quadVertexBuffer;
    QOpenGLBuffer m_quadIndexBuffer;

    QOpenGLBuffer m_cubeVertexBuffer;
    QOpenGLBuffer m_cubeIndexBuffer;
};