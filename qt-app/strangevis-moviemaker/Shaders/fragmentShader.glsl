#version 450

uniform vec4 color;
uniform sampler3D texture1;
uniform float zCoord;

in vec2 texCoords;
out vec4 fragColor;

void main(void)
{
	vec4 tex = texture(texture1, vec3(texCoords,zCoord));

	if (tex.r > 0.4) fragColor = vec4(tex.r,0.0,0.0, 1.0);
	else fragColor = vec4(tex.rrr, 1.0);
}