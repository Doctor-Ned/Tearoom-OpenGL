#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

uniform mat4 model;
uniform vec3 viewPosition;

layout (std140) uniform ViewProjection {
	mat4 view;
	mat4 projection;
};

//%lights.glsl%

out VS_OUT {
	vec3 pos;
	vec2 texCoords;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
	mat3 TBN;
} vs_out;

void main() {
    vs_out.texCoords = inTexCoord;
    vs_out.pos = vec3(model * vec4(inPosition, 1.0f));
	vs_out.normal = normalize(transpose(inverse(mat3(model))) * inNormal);
	vec3 T = normalize(vec3(model * vec4(inTangent, 0.0f)));
	vec3 B = normalize(vec3(model * vec4(inBitangent, 0.0f)));
	vec3 N = normalize(vec3(model * vec4(inNormal, 0.0f)));
	vs_out.TBN = mat3(T, B, N);
	vs_out.viewPosition = vec3(model * vec4(viewPosition, 1.0f));
	for(int i=0;i<dirLights;i++) {
		vs_out.fragDirSpaces[i] = dirLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	for(int i=0;i<spotLights;i++) {
		vs_out.fragSpotSpaces[i] = spotLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
    gl_Position = projection * view * vec4(vs_out.pos, 1.0f);
}