#version 330 core

out vec4 outColor;

in vec2 exPos;
in vec2 exTexCoords;

uniform sampler2D text;
uniform float exposure;
uniform float gamma;
uniform int useHdr;

void main() {
	vec3 color = texture(text, exTexCoords).rgb;
	if (useHdr > 0) {
		vec3 mapped = vec3(1.0) - exp(-color * exposure);
		mapped = pow(mapped, vec3(1.0 / gamma));
		outColor = vec4(mapped, 1.0f);
	} else {
		outColor = vec4(color, 1.0f);
	}
}