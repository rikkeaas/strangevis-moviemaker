#version 450

uniform mat4 mvpMatrix;
in vec4 vertexPosition;
out vec2 texCoords;

void main(void)
{
	gl_Position = mvpMatrix * vertexPosition;
	texCoords = (vertexPosition.xy + vec2(1.0)) * 0.5;
}