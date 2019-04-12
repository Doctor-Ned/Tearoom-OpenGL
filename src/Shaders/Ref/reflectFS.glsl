#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform samplerCube environmentMap;

in VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
	vec3 reflection;
} fs_in;

void main() {
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
	FragColor = vec4(texture(environmentMap, fs_in.reflection).rgb, 1.0);
}