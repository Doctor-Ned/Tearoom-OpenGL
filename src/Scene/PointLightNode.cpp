#include "PointLightNode.h"

PointLightNode::PointLightNode(PointLight * light, Mesh *mesh, GraphNode *parent) : QuadraticLightNode(mesh, parent) {
	this->name = "Point light";
	this->light = light;
}

glm::vec4 PointLightNode::getAmbient() {
	return light->ambient;
}

glm::vec4 PointLightNode::getDiffuse() {
	return light->diffuse;
}

glm::vec4 PointLightNode::getSpecular() {
	return light->specular;
}

float PointLightNode::getConstant() {
	return light->constant;
}

float PointLightNode::getLinear() {
	return light->linear;
}

float PointLightNode::getQuadratic() {
	return light->quadratic;
}

void PointLightNode::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void PointLightNode::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void PointLightNode::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

void PointLightNode::setConstant(float constant) {
	light->constant = constant;
}

void PointLightNode::setLinear(float linear) {
	light->linear = linear;
}

void PointLightNode::setQuadratic(float quadratic) {
	light->quadratic = quadratic;
}

PointLight* PointLightNode::getLight() const {
	return light;
}

void PointLightNode::setModel(glm::mat4 model) {
	light->model = model;
}
