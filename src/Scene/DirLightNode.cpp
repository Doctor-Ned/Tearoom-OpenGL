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

void DirLightNode::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void DirLightNode::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void DirLightNode::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

DirLight* DirLightNode::getLight() const {
	return light;
}

void DirLightNode::setModel(glm::mat4 model) {
	light->model = model;
}
