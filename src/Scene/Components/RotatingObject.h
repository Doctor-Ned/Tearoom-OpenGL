#pragma once
#include "Component.h"

class RotatingObject : public Component {
public:
	RotatingObject(float rotationSpeed, GraphNode *gameObject);
	void update(float timeDiff) override;
	virtual void setRotationSpeed(float speed);
	float getRotationSpeed();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
protected:
	float rotationSpeed;
	RotatingObject(){}
	friend class Serializer;
};
