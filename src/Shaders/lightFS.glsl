#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT{
	vec3 pos;
	vec3 normal;
} fs_in;

uniform vec4 color;

void main() {
	FragColor = color;
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = color;
}