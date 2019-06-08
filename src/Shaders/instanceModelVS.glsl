#version 330 core

// to be used with modelFragmentShader.glsl

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in vec3 inOffset;

uniform mat4 model;
uniform vec3 viewPosition;
uniform float textureTileX;
uniform float textureTileY;

layout (std140) uniform ViewProjection {
	mat4 view;
	mat4 projection;
};

//%lights.glsl%

float density = 0.008f;

out VS_OUT {
	vec3 pos;
	vec2 texCoords;
	vec3 normal;
	vec3 viewPosition;
	vec4 fragDirSpaces[MAX_LIGHTS_OF_TYPE];
	vec4 fragSpotSpaces[MAX_LIGHTS_OF_TYPE];
	mat3 TBN;
	vec3 TanViewPos;
	vec3 TanFragPos;
	float visibility;
} vs_out;

void main() {
	vs_out.texCoords = vec2(inTexCoord.x * textureTileX, inTexCoord.y * textureTileY);
    vs_out.pos = vec3(model * vec4(inPosition+inOffset, 1.0f));
	vs_out.normal = normalize(vec3(model * vec4(inNormal, 0.0f)));
	vs_out.viewPosition = viewPosition;
	
	vec3 T = normalize(vec3(model * vec4(inTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(inNormal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	vs_out.TBN = mat3(T, B, N);

	vs_out.TanViewPos = transpose(vs_out.TBN) * viewPosition;
	vs_out.TanFragPos = transpose(vs_out.TBN) * vs_out.pos;

	for(int i=0;i<dirLights;i++) {
		vs_out.fragDirSpaces[i] = dirLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	for(int i=0;i<spotLights;i++) {
		vs_out.fragSpotSpaces[i] = spotLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	//fog
	vec4 positionRelativeToViewMat = view * vec4(vs_out.pos, 1.0f);
	float distance = length(positionRelativeToViewMat.xyz);
	vs_out.visibility = exp(-distance * density);

	gl_Position = projection * positionRelativeToViewMat;
}