#version 330 core

in vec4 FragPos;
uniform float far_plane;
uniform float near_plane;
uniform vec3 lightPos;

void main() { 
	float lightDistance = distance(FragPos.xyz, lightPos);
	lightDistance = (lightDistance-near_plane) / (far_plane-near_plane);
	gl_FragDepth = lightDistance;
}