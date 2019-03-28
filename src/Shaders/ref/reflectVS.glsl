#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4 model;
uniform float scale;
uniform vec3 viewPosition;

layout (std140) uniform ViewProjection {
	mat4 view;
	mat4 projection;
};

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
	vec3 reflection;
} vs_out;

void main() {
	vec3 pos = inPosition * scale;
    vs_out.pos = vec3(model * vec4(pos, 1.0f));
	vs_out.normal = normalize(vec3(model * vec4(inNormal, 0.0f)));
	vs_out.viewPosition = vec3(model * vec4(viewPosition, 1.0f));
	vec3 viewDirection = normalize(vs_out.pos - vs_out.viewPosition);
	vs_out.reflection = reflect(viewDirection, vs_out.normal);
    gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
}