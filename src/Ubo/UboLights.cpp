#include "UboLights.h"

UboLights::UboLights(float ambient, int dirLights, int spotLights, int pointLights, DirLight * dirLight, SpotLight * spotLight, PointLight * pointLight) :
	Ubo(sizeof(float) + 3 * sizeof(int) + MAX_LIGHTS_OF_TYPE * sizeof(DirLight) + MAX_LIGHTS_OF_TYPE * sizeof(SpotLight) + MAX_LIGHTS_OF_TYPE * sizeof(PointLight), "Lights", 5) {
	inject(ambient, dirLights, spotLights, pointLights, dirLight, spotLight, pointLight);
}

void UboLights::inject(float ambient, int dirLights, int spotLights, int pointLights, DirLight* dirLight, SpotLight* spotLight, PointLight* pointLight) {
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
	if (dirLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(dirLights, MAX_LIGHTS_OF_TYPE) * sizeof(DirLight), dirLight);
	}
	offset += MAX_LIGHTS_OF_TYPE * sizeof(DirLight);
	if (spotLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(spotLights, MAX_LIGHTS_OF_TYPE) * sizeof(SpotLight), spotLight);
	}
	offset += MAX_LIGHTS_OF_TYPE * sizeof(SpotLight);
	if (pointLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(pointLights, MAX_LIGHTS_OF_TYPE) * sizeof(PointLight), pointLight);
	}
}
