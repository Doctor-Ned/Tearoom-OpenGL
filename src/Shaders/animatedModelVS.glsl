#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in ivec4 inBoneIDs;
layout (location = 6) in ivec4 inBoneIDs2;
layout (location = 7) in vec4 inBoneWages;
layout (location = 8) in vec4 inBoneWages2;
layout (location = 9) in int inBoneCounter;

#define MAX_BONE_TRANSFORMS 80 // make sure to sync this with the value in Shader.h!

uniform mat4 model;
uniform vec3 viewPosition;
uniform mat4 boneTransforms[MAX_BONE_TRANSFORMS];
uniform float textureTileX;
uniform float textureTileY;

layout(std140) uniform ViewProjection{
	mat4 view;
	mat4 projection;
};

//%lights.glsl%

out VS_OUT{
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

float density = 0.04f;

void main() {

	vec4 vertex = vec4(inPosition, 1.0f);

	vec4 newPosition = vec4(0.0f);
	vec4 newNormal = vec4(0.0f);

	mat4 boneTransform = boneTransforms[inBoneIDs[0]] * inBoneWages[0];
	for (int i = 1; i < inBoneCounter; i++) {
		if (i < 4) {
			boneTransform += boneTransforms[inBoneIDs[i]] * inBoneWages[i];
		} else {
			boneTransform += boneTransforms[inBoneIDs2[i - 4]] * inBoneWages2[i - 4];
		}
	}
	newPosition = boneTransform * vertex;
	newNormal = boneTransform * vec4(inNormal, 0.0f);
	newPosition.w = 1.0f;

	vs_out.texCoords = vec2(inTexCoord.x * textureTileX, inTexCoord.y * textureTileY);
	vs_out.pos = vec3(model * newPosition);
	vs_out.normal = normalize(transpose(inverse(mat3(model))) * newNormal.xyz);
	vs_out.viewPosition = vec3(model * vec4(viewPosition, 1.0f));
	
	vec3 T = normalize(vec3(model * vec4(inTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(inNormal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	vs_out.TBN = mat3(T, B, N);

	vs_out.TanViewPos = transpose(vs_out.TBN) * viewPosition;
	vs_out.TanFragPos = transpose(vs_out.TBN) * vs_out.pos; 
	
	for (int i = 0; i < dirLights; i++) {
		vs_out.fragDirSpaces[i] = dirLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	for (int i = 0; i < spotLights; i++) {
		vs_out.fragSpotSpaces[i] = spotLight[i].lightSpace * vec4(vs_out.pos, 1.0f);
	}
	//fog
	vec4 positionRelativeToViewMat = view * vec4(vs_out.pos, 1.0f);
	float distance = length(positionRelativeToViewMat.xyz);
	vs_out.visibility = exp(-pow((distance * density), 2));
	vs_out.visibility = clamp(vs_out.visibility, 0.0f, 1.0f);

	gl_Position = projection * positionRelativeToViewMat;
}