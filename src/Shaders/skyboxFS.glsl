#version 430
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in vec3 texCoords;
out vec4 outColor;

uniform samplerCube skybox;

void main() {
	FragColor = texture(skybox, texCoords);
	BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}