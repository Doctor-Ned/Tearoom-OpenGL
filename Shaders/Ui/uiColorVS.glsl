#version 330 core
layout(location = 0) in vec2 vertex;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(vertex, 0.0, 1.0);
}