#include "UboLights.h"
#include "Render/LightManager.h"

UboLights::UboLights(float ambient, int dirLights, int spotLights, int pointLights, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight * dirLight, SpotLight * spotLight, PointLight * pointLight) :
	Ubo(sizeof(float) + 7 * sizeof(int) + MAX_LIGHTS_OF_TYPE * (sizeof(DirLight)-sizeof(void*)) + MAX_LIGHTS_OF_TYPE * (sizeof(SpotLight) - sizeof(void*)) + MAX_LIGHTS_OF_TYPE * (sizeof(PointLight) - sizeof(void*)), "Lights", 5) {
	inject(ambient, dirLights, spotLights, pointLights, spotDirShadowTexelResolution, pointShadowSamples, dirLight, spotLight, pointLight);
}

void UboLights::inject(float ambient, int dirLights, int spotLights, int pointLights, int spotDirShadowTexelResolution, int pointShadowSamples, DirLight* dirLight, SpotLight* spotLight, PointLight* pointLight) {
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
	offset += 3 * sizeof(int);
	if (dirLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(dirLights, MAX_LIGHTS_OF_TYPE) * (sizeof(DirLight) - sizeof(void*)), dirLight);
	}
	offset += MAX_LIGHTS_OF_TYPE * (sizeof(DirLight) - sizeof(void*));
	if (spotLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(spotLights, MAX_LIGHTS_OF_TYPE) * (sizeof(SpotLight) - sizeof(void*)), spotLight);
	}
	offset += MAX_LIGHTS_OF_TYPE * (sizeof(SpotLight) - sizeof(void*));
	if (pointLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(pointLights, MAX_LIGHTS_OF_TYPE) * (sizeof(PointLight) - sizeof(void*)), pointLight);
	}
}
