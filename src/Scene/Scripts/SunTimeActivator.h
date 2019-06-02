#pragma once
#include "Scene/Components/LightComponents/Sun.h"
#include "Picking.h"

class SunTimeActivator : public Component
{
protected:
	friend class Serializer;
	SunTimeActivator(){}
	void renderGui() override;
	Sun *sun = nullptr;
	std::vector<Component*> components;
	int targetTime = 12;
	bool correct = false;
public:
	Sun *getSun();
	void setSun(Sun *sun);
	std::vector<Component*> getActivatableComponents();
	void addActivatableComponent(Component *component);
	void removeActivatableComponent(Component *component);
	void clearActivatableComponents();
	bool hasComponent(Component *comp);
	bool isTimeCorrect();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	SunTimeActivator(GraphNode* _gameObject);
	void update(float msec) override;
};
