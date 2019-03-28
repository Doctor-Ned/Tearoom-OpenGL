#version 330 core

out vec4 outColor;

in vec2 exPos;
in vec2 exTexCoords;

uniform sampler2D text;
uniform int enabled;

void main() {
	vec3 color = vec3(texture(text, exTexCoords));
	if(enabled > 0) {
		color = vec3(1.0f) - color;
	}
	outColor = vec4(color, 1.0f);
}