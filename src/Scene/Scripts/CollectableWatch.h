#pragma once

#include "Scene/Components/Component.h"

class SunController;

class CollectableWatch : public Component {
public:
	CollectableWatch(GraphNode* _gameObject);
	void update(float msec) override;
	void pickup();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	void setSunController(SunController *sunController);
	SunController *getSunController();
protected:
	void renderGui() override;
	SunController *sunController = nullptr;
	CollectableWatch(){}
	friend class Serializer;
};
