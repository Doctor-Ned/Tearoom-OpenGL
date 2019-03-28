#include "UboLights.h"

UboLights::UboLights(DirLight * dirLight, SpotLight * spotLight, PointLight * pointLight) : Ubo(sizeof(DirLight) + sizeof(SpotLight) + sizeof(PointLight), "Lights", 5) {
	inject(dirLight, spotLight, pointLight);
}

void UboLights::inject(DirLight* dirLight, SpotLight* spotLight, PointLight* pointLight) {
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	size_t offset = 0;
	if (dirLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(DirLight), dirLight);
	offset += sizeof(DirLight);
	if (spotLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(SpotLight), spotLight);
	offset += sizeof(SpotLight);
	if (pointLight != nullptr) glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(PointLight), pointLight);
}
