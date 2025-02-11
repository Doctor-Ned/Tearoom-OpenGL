#version 330 core

out vec4 outColor;

in vec2 exPos;
in vec2 exTexCoords;

#define BLOOM_TEXTURES 4

uniform sampler2D scene;
uniform sampler2D bloom[BLOOM_TEXTURES];
uniform float exposure;
uniform float gamma;
uniform bool useHdr;
uniform bool useBloom;
uniform bool useAntialiasing;
uniform vec2 inverseScreenSize;
uniform vec2 screenSize;

const float fxaaSpanMax = 8.0f;
const float fxaaReduceMin = 1.0f / 128.0f;
const float fxaaReduceMul = 1.0f / 4.0f;

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

//#define USE_WEIRD_FILTER

vec3 getAntialiasedColor() {
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaTL = dot(luma, texture(scene, exTexCoords + (vec2(-1.0, -1.0) * inverseScreenSize)).rgb);
	float lumaTR = dot(luma, texture(scene, exTexCoords + (vec2(1.0, -1.0) * inverseScreenSize)).rgb);
	float lumaBL = dot(luma, texture(scene, exTexCoords + (vec2(-1.0, 1.0) * inverseScreenSize)).rgb);
	float lumaBR = dot(luma, texture(scene, exTexCoords + (vec2(1.0, 1.0) * inverseScreenSize)).rgb);
	float lumaM = dot(luma, texture(scene, exTexCoords).rgb);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
	float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(fxaaSpanMax, fxaaSpanMax),
			  max(vec2(-fxaaSpanMax, -fxaaSpanMax), dir * inverseDirAdjustment)) * inverseScreenSize;

	vec3 result1 = (1.0 / 2.0) * (
		texture(scene, exTexCoords + (dir * vec2(1.0 / 3.0 - 0.5))).rgb +
		texture(scene, exTexCoords + (dir * vec2(2.0 / 3.0 - 0.5))).rgb);

	vec3 result2 = result1 * (1.0 / 2.0) + (1.0 / 4.0) * (
		texture(scene, exTexCoords + (dir * vec2(0.0 / 3.0 - 0.5))).rgb +
		texture(scene, exTexCoords + (dir * vec2(3.0 / 3.0 - 0.5))).rgb);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);

	if (lumaResult2 < lumaMin || lumaResult2 > lumaMax) {
		return result1;
	}
	return result2;
}

vec3 uncharted2Tonemap(vec3 x) {
	return ((x*(A*x + C * B) + D * E) / (x*(A*x + B) + D * F)) - E / F;
}

vec4 applyTonemapping(vec3 texColor) {
	texColor *= 16;  // Hardcoded Exposure Adjustment

	vec3 curr = uncharted2Tonemap(exposure*texColor);

	vec3 whiteScale = uncharted2Tonemap(vec3(W, W, W));
	whiteScale.r = 1.0f / whiteScale.r;
	whiteScale.g = 1.0f / whiteScale.g;
	whiteScale.b = 1.0f / whiteScale.b;
	vec3 color = curr * whiteScale;

	color.r = pow(color.r, 1.0f / gamma);
	color.g = pow(color.g, 1.0f / gamma);
	color.b = pow(color.b, 1.0f / gamma);

	return vec4(color, 1.0f);
}

vec4 vignette(vec4 color) {
	vec2 position = (gl_FragCoord.xy / screenSize.xy) - vec2(0.5f);
	float len = length(position);
	float r = 0.94f;
	float softness = 0.7f;

	return color * vec4(vec3(smoothstep(r, r - softness, len)), 1.0);
}

vec3 getWeirdFilter() {
	vec3 color = vec3(0.0f);

	const float Gx1[5] = float[] ( 2.0f, 2.0f, 4.0f, 2.0f, 2.0f );
	const float Gx2[5] = float[] ( 1.0f, 1.0f, 2.0f, 1.0f, 1.0f );
	const float Gx3[5] = float[] ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	const float Gx4[5] = float[] ( -1.0f, -1.0f, -2.0f, -1.0f, -1.0f );
	const float Gx5[5] = float[] ( -2.0f, -2.0f, -4.0f, -2.0f, -2.0f );

	for (int i = -2; i < 3; i++) {
		color += texture(scene, exTexCoords + vec2(i, -2.0f) * inverseScreenSize).rgb * Gx1[i];
		color += texture(scene, exTexCoords + vec2(i, -1.0f) * inverseScreenSize).rgb * Gx2[i];
		color += texture(scene, exTexCoords + vec2(i, 1.0f) * inverseScreenSize).rgb * Gx4[i];
		color += texture(scene, exTexCoords + vec2(i, 2.0f) * inverseScreenSize).rgb * Gx5[i];

		color += texture(scene, exTexCoords + vec2(-2.0f, i) * inverseScreenSize).rgb * Gx1[i];
		color += texture(scene, exTexCoords + vec2(-1.0f, i) * inverseScreenSize).rgb * Gx2[i];
		color += texture(scene, exTexCoords + vec2(1.0f, i) * inverseScreenSize).rgb * Gx4[i];
		color += texture(scene, exTexCoords + vec2(2.0f, i) * inverseScreenSize).rgb * Gx5[i];
	}

	//color += texture(scene, exTexCoords + vec2(2.0, 0.0) * inverseScreenSize).rgb * 4.0f;
	//color += texture(scene, exTexCoords + vec2(1.0, 0.0) * inverseScreenSize).rgb * 2.0f;
	//color += texture(scene, exTexCoords + vec2(-1.0, 0.0) * inverseScreenSize).rgb * -2.0f;
	//color += texture(scene, exTexCoords + vec2(-2.0, 0.0) * inverseScreenSize).rgb * -4.0f;

	//color += texture(scene, exTexCoords + vec2(0.0, 2.0) * inverseScreenSize).rgb * 4.0f;
	//color += texture(scene, exTexCoords + vec2(0.0, 1.0) * inverseScreenSize).rgb * 2.0f;
	//color += texture(scene, exTexCoords + vec2(0.0, -1.0) * inverseScreenSize).rgb * -2.0f;
	//color += texture(scene, exTexCoords + vec2(0.0, -2.0) * inverseScreenSize).rgb * -4.0f;

	return color;
}

void main() {
#ifdef USE_WEIRD_FILTER
	vec3 hdrColor = getWeirdFilter();
#else
	vec3 hdrColor = useAntialiasing ? getAntialiasedColor() : texture(scene, exTexCoords).rgb;
#endif

		if (useBloom) {
			for (int i = 0; i < BLOOM_TEXTURES; i++) {
				hdrColor += texture(bloom[i], exTexCoords).rgb;
			}
		}
		if (useHdr) {
			outColor = applyTonemapping(hdrColor);
		} else {
			outColor = vec4(hdrColor, 1.0);
		}

	outColor = vignette(outColor);
}