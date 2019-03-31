#include "UboLights.h"

UboLights::UboLights(float ambient, int dirLights, int spotLights, int pointLights, DirLight ** dirLight, SpotLight ** spotLight, PointLight ** pointLight) :
	Ubo(sizeof(float) + 3 * sizeof(int) + MAX_LIGHTS_OF_TYPE * sizeof(DirLight) + MAX_LIGHTS_OF_TYPE * sizeof(SpotLight) + MAX_LIGHTS_OF_TYPE * sizeof(PointLight), "Lights", 5) {
	inject(ambient, dirLights, spotLights, pointLights, dirLight, spotLight, pointLight);
}

void UboLights::inject(float ambient, int dirLights, int spotLights, int pointLights, DirLight** dirLight, SpotLight** spotLight, PointLight** pointLight) {
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
	std::vector<DirLight> dirs;
	for(int i=0;i<dirLights;i++) {
		dirs.emplace_back(*dirLight[i]);
	}
	std::vector<SpotLight> spots;
	for (int i = 0; i < spotLights; i++) {
		spots.emplace_back(*spotLight[i]);
	}
	std::vector<PointLight> points;
	for (int i = 0; i < pointLights; i++) {
		points.emplace_back(*pointLight[i]);
	}
	if (dirLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(dirLights, MAX_LIGHTS_OF_TYPE) * sizeof(DirLight), &dirs[0]);
	}
	offset += MAX_LIGHTS_OF_TYPE * sizeof(DirLight);
	if (spotLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(spotLights, MAX_LIGHTS_OF_TYPE) * sizeof(SpotLight), &spots[0]);
	}
	offset += MAX_LIGHTS_OF_TYPE * sizeof(SpotLight);
	if (pointLights > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, glm::min(pointLights, MAX_LIGHTS_OF_TYPE) * sizeof(PointLight), &points[0]);
	}
}
