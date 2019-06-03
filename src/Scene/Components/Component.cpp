#include "Component.h"
#include "Serialization/Serializer.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Profiler.h"

Component::Component() {
	gameManager = GameManager::getInstance();
}

Component::Component(GraphNode* _gameObject, std::string name) : gameObject(_gameObject), name(name) {
	gameManager = GameManager::getInstance();
}

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

void Component::updateComponent(float msec)
{
	double  startTime = glfwGetTime();
	update(msec);
	double elapsedTime = glfwGetTime() - startTime;
	if(getGameObject() != nullptr)
	{
		Profiler::getInstance()->addUpdateTime(getGameObject()->getName() + " : " + getName(), elapsedTime * 1000.0f);
	}
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
	static const int BUFF_SIZE = 50;
	static char buff[BUFF_SIZE] = "";
	if (buff[0] == '\0') {
		for (int i = 0; i < std::min(BUFF_SIZE, static_cast<int>(name.length())); i++) {
			buff[i] = name[i];
		}
	}
	ImGui::InputText("Name", buff, IM_ARRAYSIZE(buff));
	std::string newName(Global::trim(buff));
	if (newName.length() > 0 && newName != name && ImGui::Button("Apply name")) {
		setName(newName);
		buff[0] = '\0';
	}
}
