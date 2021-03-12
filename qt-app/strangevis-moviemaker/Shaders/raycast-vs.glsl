#version 450

in vec2 position;
out vec2 fragPosition;

out vec2 pp;
void main()
{
	fragPosition = position;
	gl_Position = vec4(position,0.0,1.0);
}