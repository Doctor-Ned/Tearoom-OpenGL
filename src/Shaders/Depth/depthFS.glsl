#version 330 core

out vec4 outColor;

uniform float opacity;

void main() {
	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25f * (dx * dx + dy * dy);
	outColor = vec4(depth, moment2, opacity, 0.0f);
}