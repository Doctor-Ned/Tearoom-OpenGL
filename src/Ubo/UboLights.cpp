#include "UboLights.h"

UboLights::UboLights(float ambient, DirLight * dirLight, SpotLight * spotLight, PointLight * pointLight) :
Ubo(sizeof(float) * 4 + sizeof(DirLight) + sizeof(SpotLight) + sizeof(PointLight), "Lights", 5) {
	inject(ambient, dirLight, spotLight, pointLight);
}

void UboLights::inject(float ambient, DirLight* dirLight, SpotLight* spotLight, PointLight* pointLight) {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	size_t offset = 0;
	glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &ambient);
	offset += 4 * sizeof(float);
	if (dirLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(DirLight), dirLight);
	offset += sizeof(DirLight);
	if (spotLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(SpotLight), spotLight);
	offset += sizeof(SpotLight);
	if (pointLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLight), pointLight);
}
