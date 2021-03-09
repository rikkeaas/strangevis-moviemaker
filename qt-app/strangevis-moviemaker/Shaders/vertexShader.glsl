uniform mat4 mvpMatrix;
in vec4 vertex;
out vec2 texCoords;
void main(void)
{
	gl_Position = mvpMatrix * vertex;
	texCoords = (vertex.xy + vec2(1.0)) * 0.5;
}