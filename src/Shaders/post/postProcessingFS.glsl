#version 330 core

out vec4 outColor;

in vec2 exPos;
in vec2 exTexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D ui;
uniform float exposure;
uniform float gamma;
uniform bool useHdr;
uniform bool bloom;

void main() {
	vec4 uiColor = texture(ui, exTexCoords);
	if (uiColor.a > 0.0f) {
		outColor = uiColor;
	} else {
		vec3 hdrColor = texture(scene, exTexCoords).rgb;
		vec3 bloomColor = texture(bloomBlur, exTexCoords).rgb;
		if (bloom) {
			hdrColor += bloomColor;
		}
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		result = pow(result, vec3(1.0 / gamma));
		outColor = vec4(result, 1.0);
	}
}