#include "DirLightComp.h"

DirLightComp::DirLightComp(DirLight * light, GraphNode *gameObject) : LightComp(gameObject) {
	this->light = light;
}

glm::vec4 DirLightComp::getAmbient() {
	return light->ambient;
}

glm::vec4 DirLightComp::getDiffuse() {
	return light->diffuse;
}

glm::vec4 DirLightComp::getSpecular() {
	return light->specular;
}

void DirLightComp::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void DirLightComp::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void DirLightComp::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

DirLight* DirLightComp::getLight() const {
	return light;
}

bool DirLightComp::getEnabled() {
	return light->enabled;
}

void DirLightComp::setEnabled(bool enabled) {
	light->enabled = enabled ? 1 : 0;
}

void DirLightComp::setModel(glm::mat4 model) {
	light->model = model;
}
