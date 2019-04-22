#include "SpotLightComp.h"
#include "Render/LightManager.h"
#include "Serialization/Serializer.h"

SpotLightComp::SpotLightComp(SpotLight * light, GraphNode *gameObject) : QuadraticLightComp(gameObject) {
	this->light = light;
}

glm::vec4 SpotLightComp::getAmbient() {
	return light->ambient;
}

glm::vec4 SpotLightComp::getDiffuse() {
	return light->diffuse;
}

glm::vec4 SpotLightComp::getSpecular() {
	return light->specular;
}

float SpotLightComp::getConstant() {
	return light->constant;
}

float SpotLightComp::getLinear() {
	return light->linear;
}

float SpotLightComp::getQuadratic() {
	return light->quadratic;
}

float SpotLightComp::getCutoff() {
	return light->cutOff;
}

float SpotLightComp::getOuterCutoff() {
	return light->outerCutOff;
}

void SpotLightComp::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void SpotLightComp::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void SpotLightComp::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

void SpotLightComp::setConstant(float constant) {
	light->constant = constant;
}

void SpotLightComp::setLinear(float linear) {
	light->linear = linear;
}

void SpotLightComp::setQuadratic(float quadratic) {
	light->quadratic = quadratic;
}

void SpotLightComp::setCutoff(float cutoff) {
	light->cutOff = cutoff;
}

void SpotLightComp::setOuterCutoff(float outerCutoff) {
	light->outerCutOff = outerCutoff;
}

SpotLight* SpotLightComp::getLight() const {
	return light;
}

SerializableType SpotLightComp::getSerializableType() {
	return SSpotLightComp;
}

Json::Value SpotLightComp::serialize(Serializer* serializer) {
	Json::Value root = LightComp::serialize(serializer);
	root["light"] = serializer->serialize(light);
	return root;
}

void SpotLightComp::deserialize(Json::Value& root, Serializer* serializer) {
	LightComp::deserialize(root, serializer);
	light = dynamic_cast<SpotLight*>(serializer->deserialize(root["light"]).object);
}

bool SpotLightComp::getEnabled() {
	return light->enabled;
}

void SpotLightComp::setEnabled(bool enabled) {
	light->enabled = enabled ? 1 : 0;
}

void SpotLightComp::setModel(glm::mat4 model) {
	light->model = model;
}

void SpotLightComp::renderGui() {
	QuadraticLightComp::renderGui();
	if (active) {
		ImGui::SliderAngle("Cutoff", &light->cutOff, 0.0f, 360.0f);
		ImGui::SliderAngle("Outer cutoff", &light->outerCutOff, 0.0f, 360.0f);
	}
}
