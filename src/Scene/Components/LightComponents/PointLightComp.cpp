#include "PointLightComp.h"
#include "Render/LightManager.h"
#include "Serialization/Serializer.h"

PointLightComp::PointLightComp(PointLight * light, GraphNode *gameObject) : QuadraticLightComp(gameObject) {
	this->light = light;
}

glm::vec4 PointLightComp::getAmbient() {
	return light->ambient;
}

glm::vec4 PointLightComp::getDiffuse() {
	return light->diffuse;
}

glm::vec4 PointLightComp::getSpecular() {
	return light->specular;
}

float PointLightComp::getConstant() {
	return light->constant;
}

float PointLightComp::getLinear() {
	return light->linear;
}

float PointLightComp::getQuadratic() {
	return light->quadratic;
}

void PointLightComp::setAmbient(glm::vec4 ambient) {
	light->ambient = ambient;
}

void PointLightComp::setDiffuse(glm::vec4 diffuse) {
	light->diffuse = diffuse;
}

void PointLightComp::setSpecular(glm::vec4 specular) {
	light->specular = specular;
}

void PointLightComp::setConstant(float constant) {
	light->constant = constant;
}

void PointLightComp::setLinear(float linear) {
	light->linear = linear;
}

void PointLightComp::setQuadratic(float quadratic) {
	light->quadratic = quadratic;
}

PointLight* PointLightComp::getLight() const {
	return light;
}

SerializableType PointLightComp::getSerializableType() {
	return SPointLightComp;
}

Json::Value PointLightComp::serialize(Serializer* serializer) {
	Json::Value root = LightComp::serialize(serializer);
	root["light"] = serializer->serialize(light);
	return root;
}

void PointLightComp::deserialize(Json::Value& root, Serializer* serializer) {
	light = dynamic_cast<PointLight*>(serializer->deserialize(root["light"]).object);
	LightComp::deserialize(root, serializer);
}

bool PointLightComp::getEnabled() {
	return light->enabled;
}

void PointLightComp::setEnabled(bool enabled) {
	light->enabled = enabled ? 1 : 0;
}

void PointLightComp::setModel(glm::mat4 model) {
	light->model = model;
}
