#version 430
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform samplerCube environmentMap;

in VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
	vec3 refraction;
} fs_in;

void main() {
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
	FragColor = vec4(texture(environmentMap, fs_in.refraction).rgb, 1.0);
}