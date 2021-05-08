#version 450

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;
uniform vec3 voxelSpacing;

in vec2 vertex;
out vec2 fragCoord;

void main()
{
	fragCoord = vertex;
	gl_Position = vec4(vertex, 0.0, 1.0);
}
