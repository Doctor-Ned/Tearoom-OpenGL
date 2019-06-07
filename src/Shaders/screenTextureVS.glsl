#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 texCoords;

out vec2 exTexCoords;

void main() {
	exTexCoords = texCoords;
	gl_Position = vec4(inPosition, 0.0f, 1.0f);
}