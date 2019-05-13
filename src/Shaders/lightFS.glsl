#version 430
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT{
	vec3 pos;
	vec3 normal;
} fs_in;

uniform vec4 color;
uniform float opacity;

void main() {
	FragColor = vec4(color.rgb, opacity);
	BrightColor = FragColor;
}