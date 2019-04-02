#include "DirLightNode.h"

DirLightNode::DirLightNode(DirLight * light, Mesh *mesh, GraphNode *parent) : LightNode(mesh, parent) {
	this->name = "Dir light";
	this->light = light;
}

glm::vec4 DirLightNode::getAmbient() {
	return light->ambient;
}

glm::vec4 DirLightNode::getDiffuse() {
	return light->diffuse;
}

glm::vec4 DirLightNode::getSpecular() {
	return light->specular;
}

bool DirLightNode::getEnabled() {
	return light->enabled;
}

void DirLightNode::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void DirLightNode::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void DirLightNode::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

void DirLightNode::setEnabled(bool enabled) {
	light->enabled = enabled;
}

DirLight* DirLightNode::getLight() const {
	return light;
}