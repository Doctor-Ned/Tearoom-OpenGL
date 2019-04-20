#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include "Scene/GuiConfigurable.h"
#include <imgui.h>

class GraphNode;
class Component abstract : public GuiConfigurable
{
protected:
	GraphNode* gameObject;
	Component(GraphNode* _gameObject, std::string name = "Component") : gameObject(_gameObject), name(name) {}
	bool active = true;
	std::string name;
public:
	void setName(std::string name) { this->name = name; }
	virtual void updateWorld() {}
	virtual bool isComponentActive() { return active; }
	virtual void setComponentActive(bool active) { this->active = active; }
	virtual void update(float msec) {}
	GraphNode* getGameObject() const { return gameObject; }
	virtual ~Component() = default;
	void renderGui() override {
		bool active = this->active;
		ImGui::Checkbox(name.c_str(), &active);
		if (active != this->active)setComponentActive(active);
		ImGui::NewLine();
	}
};
#endif