#version 450

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;

uniform vec3 voxelSpacing;

in vec2 vertex;

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
	fragCoord = vertex;
	gl_Position = vec4(vertex, 0.0, 1.0);
}

