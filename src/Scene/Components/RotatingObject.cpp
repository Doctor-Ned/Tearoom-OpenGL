#include "RotatingObject.h"
#include "Scene/GraphNode.h"

RotatingObject::RotatingObject(float rotationSpeed, GraphNode* gameObject) : Component(gameObject, "Rotating object"),
                                                                                 rotationSpeed(rotationSpeed) {}

void RotatingObject::update(float timeDiff) {
	gameObject->localTransform.rotateByRadians(float(rotationSpeed * timeDiff), glm::vec3(0.0f, 1.0f, 0.0f));
}

void RotatingObject::setRotationSpeed(float speed) {
	this->rotationSpeed = speed;
}

float RotatingObject::getRotationSpeed() {
	return rotationSpeed;
}
