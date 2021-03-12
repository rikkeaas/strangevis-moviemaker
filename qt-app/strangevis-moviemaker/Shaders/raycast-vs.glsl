#version 450

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;

in vec4 vertex;

out vec2 fragCoord;


/*
void main()
{
	gl_Position = vertex;    
	vec2 pos = gl_Position.xy/gl_Position.w;
	near = inverseModelViewProjectionMatrix * (vec4(pos, -1.0, 1.0));       
    far = inverseModelViewProjectionMatrix * (vec4(pos, +1.0, 1.0));
}*/

void main()
{
	//vec4 newVertex = modelViewProjectionMatrix * vertex;
	fragCoord = vertex.xy;
	gl_Position = vertex;
}

