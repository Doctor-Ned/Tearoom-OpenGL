#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoords;

out vec2 exTexCoords;

void main() {
	exTexCoords = inTexCoords;
    gl_Position = vec4(inPosition, 1.0f);
}