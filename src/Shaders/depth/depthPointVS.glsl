#version 330 core

layout (location = 0) in vec3 inPosition;

uniform float scale;
uniform mat4 model;

void main() {
    gl_Position = model * vec4(scale * inPosition, 1.0f);
}