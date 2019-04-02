#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

uniform mat4 model;
uniform vec3 viewPosition;

layout (std140) uniform ViewProjection {
	mat4 view;
	mat4 projection;
};

//%lights.glsl%

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
} vs_out;

void main() {
    vs_out.pos = vec3(model * vec4(inPosition, 1.0f));
	vs_out.normal = normalize(transpose(inverse(mat3(model))) * inNormal);
	vs_out.viewPosition = vec3(model * vec4(viewPosition, 1.0f));
	for(int i=0;i<dirLights;i++) {
		vs_out.fragDirSpaces[i] = dirLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	for(int i=0;i<spotLights;i++) {
		vs_out.fragSpotSpaces[i] = spotLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
    gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
}