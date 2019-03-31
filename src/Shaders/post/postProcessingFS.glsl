#version 330 core

out vec4 outColor;

in vec2 exPos;
in vec2 exTexCoords;

uniform sampler2D text;

void main() {
	vec3 color = vec3(texture(text, exTexCoords));
	outColor = vec4(color, 1.0f);
}