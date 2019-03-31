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

struct DirLight {
	mat4 lightSpace;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct PointLight {
	vec4 position;
	float constant;
	float linear;
	float quadratic;
	float near_plane;
	vec3 padding;
	float far_plane;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};

struct SpotLight {
	mat4 lightSpace;
	vec4 position;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	vec3 padding;
	float outerCutOff;
};

#define MAX_LIGHTS_OF_TYPE 4

layout (std140) uniform Lights {
	float initialAmbient;
	int dirLights;
	int spotLights;
	int pointLights;
	DirLight dirLight[MAX_LIGHTS_OF_TYPE];
	SpotLight spotLight[MAX_LIGHTS_OF_TYPE];
	PointLight pointLight[MAX_LIGHTS_OF_TYPE];
};

out VS_OUT {
	vec3 pos;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
} vs_out;

void main() {
	vec3 pos = inPosition * scale;
    vs_out.pos = vec3(model * vec4(pos, 1.0f));
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