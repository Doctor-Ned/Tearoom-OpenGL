#version 330 core

out vec4 outColor;

in vec2 exTexCoords;

uniform sampler2D map;
uniform float near_plane;
uniform float far_plane;
uniform int perspective;

float linearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() { 
	float depthValue = texture(map, exTexCoords).r;
	if(perspective == 0 ) {
		outColor = vec4(vec3(depthValue), 1.0); //orthographic
	} else {
		outColor = vec4(vec3(linearizeDepth(depthValue) / far_plane), 1.0);  //perspective
	}
}