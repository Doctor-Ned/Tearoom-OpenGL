#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4 model;
uniform vec3 viewPosition;

layout (std140) uniform ViewProjection {
	mat4 view;
	mat4 projection;
};

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
} vs_out;

void main() {
    vs_out.pos = vec3(model * vec4(inPosition, 1.0f));
	vs_out.normal = normalize(vec3(model * vec4(inNormal, 0.0f)));
	vs_out.viewPosition = viewPosition;
    gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
}