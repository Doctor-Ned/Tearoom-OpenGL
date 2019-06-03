#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include "Scene/GuiConfigurable.h"
#include <imgui.h>
#include "Serialization/Serializable.h"
#include "Global.h"
#include "GLFW/glfw3.h"

class GameManager;
class GraphNode;
class Component abstract : public GuiConfigurable, public Serializable
{
protected:
	friend class Serializer;
	GameManager *gameManager;
	Component();
	GraphNode* gameObject;
	Component(GraphNode* _gameObject, std::string name = "Component");
	bool active = true;
	std::string name;
	void renderGui() override;
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void setName(std::string name);
	virtual void updateWorld();
	virtual bool isComponentActive();
	virtual void setComponentActive(bool active);
	void updateComponent(float msec);
	virtual void update(float msec);
	GraphNode* getGameObject() const;
	std::string getName();
	virtual ~Component() = default;
};
#endif