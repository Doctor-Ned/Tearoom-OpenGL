#version 430

in vec2 exTexCoords;
out vec4 outColor;
uniform sampler2D text;
uniform float opacity;

void main() {
	outColor=texture(text, exTexCoords);
	outColor.w *= opacity;
}