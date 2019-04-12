#version 330 core

out vec4 outColor;

void main() {
	outColor = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 0.0);
}