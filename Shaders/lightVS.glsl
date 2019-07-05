#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 model;

layout(std140) uniform ViewProjection{
	mat4 view;
	mat4 projection;
};

out VS_OUT{
	vec3 pos;
	vec3 normal;
} vs_out;

void main() {
	vs_out.pos = vec3(model * vec4(inPosition, 1.0f));
	vs_out.normal = normalize(transpose(inverse(mat3(model))) * inNormal);
	gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
}