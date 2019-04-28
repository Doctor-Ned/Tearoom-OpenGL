#include "Component.h"
#include "Serialization/Serializer.h"
#include "Scene/GraphNode.h"

Component::Component(GraphNode* _gameObject, std::string name) : gameObject(_gameObject), name(name) {}

SerializableType Component::getSerializableType() {
	return SComponent;
}

Json::Value Component::serialize(Serializer* serializer) {
	Json::Value root;
	root["name"] = name;
	root["active"] = active;
	root["gameObject"] = serializer->serialize(gameObject);
	return root;
}

void Component::deserialize(Json::Value& root, Serializer* serializer) {
	setName(root["name"].asString());
	setComponentActive(root["active"].asBool());
	gameObject = dynamic_cast<GraphNode*>(serializer->deserialize(root["gameObject"]).object);
}

void Component::setName(std::string name) {
	this->name = name;
}

void Component::updateWorld() {}

bool Component::isComponentActive() {
	return active;
}

void Component::setComponentActive(bool active) {
	this->active = active;
}

void Component::update(float msec) {}

GraphNode* Component::getGameObject() const {
	return gameObject;
}

std::string Component::getName()
{
	return name;
}

void Component::renderGui() {
	bool active = this->active;
	ImGui::Checkbox(name.c_str(), &active);
	if (active != this->active)setComponentActive(active);
	ImGui::NewLine();
}
