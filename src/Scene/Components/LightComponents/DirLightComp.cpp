#include "DirLightComp.h"
#include "Render/LightManager.h"
#include "Serialization/Serializer.h"

DirLightComp::DirLightComp(DirLight * light, GraphNode *gameObject) : LightComp(gameObject) {
	this->light = light;
}

glm::vec4 DirLightComp::getColor() {
	return light->color;
}

float DirLightComp::getStrength() {
	return light->strength;
}

void DirLightComp::setColor(glm::vec4 color) {
	light->color = color;
	LightComp::setColor(color);
}

DirLight* DirLightComp::getLight() const {
	return light;
}

SerializableType DirLightComp::getSerializableType() {
	return SDirLightComp;
}

Json::Value DirLightComp::serialize(Serializer* serializer) {
	Json::Value root = LightComp::serialize(serializer);
	root["light"] = serializer->serialize(light);
	return root;
}

void DirLightComp::deserialize(Json::Value& root, Serializer* serializer) {
	light = dynamic_cast<DirLight*>(serializer->deserialize(root["light"]).object);
	LightComp::deserialize(root, serializer);
}

void DirLightComp::renderGui() {
	LightComp::renderGui();
	if(active) {
		float strength = getStrength();
		ImGui::DragFloat("Strength", &strength, 0.001f, 0.0f);
		setStrength(strength);
	}
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

void DirLightComp::setStrength(float strength) {
	light->strength = strength;
}
