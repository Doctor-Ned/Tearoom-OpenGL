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
uniform bool useBloom;
uniform bool useAntialiasing;
uniform vec2 inverseScreenSize;

const float fxaaSpanMax = 8.0f;
const float fxaaReduceMin = 1.0f / 128.0f;
const float fxaaReduceMul = 1.0f / 4.0f;

vec3 getAntialiasedColor() {
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaTL = dot(luma, texture2D(scene, exTexCoords + (vec2(-1.0, -1.0) * inverseScreenSize)).rgb);
	float lumaTR = dot(luma, texture2D(scene, exTexCoords + (vec2(1.0, -1.0) * inverseScreenSize)).rgb);
	float lumaBL = dot(luma, texture2D(scene, exTexCoords + (vec2(-1.0, 1.0) * inverseScreenSize)).rgb);
	float lumaBR = dot(luma, texture2D(scene, exTexCoords + (vec2(1.0, 1.0) * inverseScreenSize)).rgb);
	float lumaM = dot(luma, texture2D(scene, exTexCoords).rgb);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
	float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(fxaaSpanMax, fxaaSpanMax),
		max(vec2(-fxaaSpanMax, -fxaaSpanMax), dir * inverseDirAdjustment)) * inverseScreenSize;

	vec3 result1 = (1.0 / 2.0) * (
		texture2D(scene, exTexCoords + (dir * vec2(1.0 / 3.0 - 0.5))).rgb +
		texture2D(scene, exTexCoords + (dir * vec2(2.0 / 3.0 - 0.5))).rgb);

	vec3 result2 = result1 * (1.0 / 2.0) + (1.0 / 4.0) * (
		texture2D(scene, exTexCoords + (dir * vec2(0.0 / 3.0 - 0.5))).rgb +
		texture2D(scene, exTexCoords + (dir * vec2(3.0 / 3.0 - 0.5))).rgb);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);

	if (lumaResult2 < lumaMin || lumaResult2 > lumaMax) {
		return result1;
	}
	return result2;
}

void main() {
	vec4 uiColor = texture(ui, exTexCoords);
	if (uiColor.a != 1.0) {
		vec3 hdrColor = useAntialiasing ? getAntialiasedColor() : texture(scene, exTexCoords).rgb;

		vec3 bloomColor = texture(bloomBlur, exTexCoords).rgb;
		if (useBloom) {
			hdrColor += bloomColor;
		}
		vec4 output;
		if (useHdr) {
			vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
			result = pow(result, vec3(1.0 / gamma));
			output = vec4(result, 1.0);
		} else {
			output = vec4(hdrColor, 1.0);
		}

		outColor = uiColor + output * (1.0 - uiColor.a);
	} else {
		outColor = uiColor;
	}
}