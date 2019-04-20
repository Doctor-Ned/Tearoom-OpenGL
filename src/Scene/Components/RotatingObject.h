#pragma once
#include "Component.h"

class RotatingObject : public Component {
public:
	RotatingObject(float rotationSpeed, GraphNode *gameObject);
	void update(float timeDiff) override;
	virtual void setRotationSpeed(float speed);
	float getRotationSpeed();
protected:
	float rotationSpeed;
};
