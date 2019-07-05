#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 exTexCoords;

uniform sampler2D text;

void main() {
	FragColor = texture(text, exTexCoords);
}