#include "SpotLightNode.h"

SpotLightNode::SpotLightNode(SpotLight * light, Mesh *mesh, GraphNode *parent) : QuadraticLightNode(mesh, parent) {
	this->name = "Spot light";
	this->light = light;
}

glm::vec4 SpotLightNode::getAmbient() {
	return light->ambient;
}

glm::vec4 SpotLightNode::getDiffuse() {
	return light->diffuse;
}

glm::vec4 SpotLightNode::getSpecular() {
	return light->specular;
}

float SpotLightNode::getConstant() {
	return light->constant;
}

float SpotLightNode::getLinear() {
	return light->linear;
}

float SpotLightNode::getQuadratic() {
	return light->quadratic;
}

float SpotLightNode::getCutoff() {
	return light->cutOff;
}

float SpotLightNode::getOuterCutoff() {
	return light->outerCutOff;
}

void SpotLightNode::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void SpotLightNode::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void SpotLightNode::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

void SpotLightNode::setConstant(float constant) {
	light->constant = constant;
}

void SpotLightNode::setLinear(float linear) {
	light->linear = linear;
}

void SpotLightNode::setQuadratic(float quadratic) {
	light->quadratic = quadratic;
}

void SpotLightNode::setCutoff(float cutoff) {
	light->cutOff = cutoff;
}

void SpotLightNode::setOuterCutoff(float outerCutoff) {
	light->outerCutOff = outerCutoff;
}

SpotLight* SpotLightNode::getLight() const {
	return light;
}

void SpotLightNode::setModel(glm::mat4 model) {
	light->model = model;
}

void SpotLightNode::renderGui() {
	QuadraticLightNode::renderGui();
	if(active) {
		ImGui::SliderAngle("Cutoff", &light->cutOff, 0.0f, 360.0f);
		ImGui::SliderAngle("Outer cutoff", &light->outerCutOff, 0.0f, 360.0f);
	}
}
