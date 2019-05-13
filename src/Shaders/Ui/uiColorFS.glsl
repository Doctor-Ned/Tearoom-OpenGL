#version 430

out vec4 outColor;

uniform vec4 color;
uniform float opacity;

void main() {
	outColor = color;
	outColor.w *= opacity;
}