#include "PointLightComp.h"
#include "Render/LightManager.h"
#include "Serialization/Serializer.h"

PointLightComp::PointLightComp(PointLight * light, GraphNode *gameObject) : QuadraticLightComp(gameObject) {
	this->light = light;
}

glm::vec4 PointLightComp::getColor() {
	return light->color;
}

float PointLightComp::getNearPlane() {
	return light->near_plane;
}

float PointLightComp::getFarPlane() {
	return light->far_plane;
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

void PointLightComp::setColor(glm::vec4 color) {
	light->color = color;
	LightComp::setColor(color);
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

void PointLightComp::renderGui() {
	QuadraticLightComp::renderGui();
	if(active) {
		float n = getNearPlane(), f = getFarPlane();
		ImGui::DragFloat("Near plane", &n, 0.01f);
		ImGui::DragFloat("Far plane", &f, 0.01f);
		if(n<0.0f) {
			n = 0.0f;
		}
		if(f<0.0f) {
			f = 0.0f;
		}
		setNearPlane(n);
		setFarPlane(f);
	}
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

void PointLightComp::setNearPlane(float nearPlane) {
	light->near_plane = nearPlane;
}

void PointLightComp::setFarPlane(float farPlane) {
	light->far_plane = farPlane;
}
