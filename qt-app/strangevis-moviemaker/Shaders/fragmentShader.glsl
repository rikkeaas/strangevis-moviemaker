#version 450

uniform vec4 color;
layout (binding=0) uniform sampler2D texture1;

in vec2 texCoords;
out vec4 fragColor;

void main(void)
{
	vec4 tex = texture(texture1, texCoords, 0);

	if (tex.r > 0.4) fragColor = vec4(tex.r,0.0,0.0, 1.0);
	else fragColor = vec4(tex.rrr, 1.0);
}