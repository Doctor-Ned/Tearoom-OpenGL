#include "UboLights.h"
#include "Render/LightManager.h"

UboLights::UboLights(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight * dirLight, SpotLight * spotLight, PointLight * pointLight) :
	Ubo(sizeof(float) + 7 * sizeof(int) + MAX_LIGHTS_OF_TYPE * (sizeof(DirLight) - sizeof(void*)) + MAX_LIGHTS_OF_TYPE * (sizeof(SpotLight) - sizeof(void*)) + MAX_LIGHTS_OF_TYPE * (sizeof(PointLight) - sizeof(void*)), "Lights", 5) {
	inject(ambient, dirLights, spotLights, pointLights, enableLights, enableShadowCasting, spotDirShadowTexelResolution, pointShadowSamples, dirLight, spotLight, pointLight);
}

void UboLights::inject(float ambient, int dirLights, int spotLights, int pointLights, bool enableLights, bool enableShadowCasting, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight* dirLight, SpotLight* spotLight, PointLight* pointLight) {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	size_t offset = 0;
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &ambient);
	offset += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &dirLights);
	offset += sizeof(int);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &spotLights);
	offset += sizeof(int);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &pointLights);
	offset += sizeof(int);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &spotDirShadowTexelResolution);
	offset += sizeof(int);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &pointShadowSamples);
	offset += sizeof(int);
	int enable = enableLights ? 1 : 0;
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &enable);
	offset += sizeof(int);
	enable = enableShadowCasting ? 1 : 0;
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &enable);
	offset += sizeof(int);
	int amount = glm::min(dirLights, MAX_LIGHTS_OF_TYPE);
	for (int i = 0; i < amount; i++) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(DirLight) - sizeof(void*), reinterpret_cast<char*>(&dirLight[i]) + sizeof(void*));
		offset += sizeof(DirLight) - sizeof(void*);
	}
	offset += (sizeof(DirLight) - sizeof(void*)) * (MAX_LIGHTS_OF_TYPE - amount);
	amount = glm::min(spotLights, MAX_LIGHTS_OF_TYPE);
	for (int i = 0; i < amount; i++) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(SpotLight) - sizeof(void*), reinterpret_cast<char*>(&spotLight[i]) + sizeof(void*));
		offset += sizeof(SpotLight) - sizeof(void*);
	}
	offset += (sizeof(SpotLight) - sizeof(void*)) * (MAX_LIGHTS_OF_TYPE - amount);
	amount = glm::min(pointLights, MAX_LIGHTS_OF_TYPE);
	for (int i = 0; i < amount; i++) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLight) - sizeof(void*), reinterpret_cast<char*>(&pointLight[i]) + sizeof(void*));
		offset += sizeof(PointLight) - sizeof(void*);
	}
}
