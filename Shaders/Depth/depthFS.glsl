#version 330 core

layout(location = 0) out vec4 outColor;

void main() {
	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25f * (dx * dx + dy * dy);
	outColor = vec4(depth, moment2, 0.0f, 1.0f);
}