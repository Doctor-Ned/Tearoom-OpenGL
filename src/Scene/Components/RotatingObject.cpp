#include "RotatingObject.h"
#include "Scene/GraphNode.h"

RotatingObject::RotatingObject(float rotationSpeed, GraphNode* gameObject) : Component(gameObject, "Rotating object"),
                                                                                 rotationSpeed(rotationSpeed) {}

void RotatingObject::update(float timeDiff) {
	gameObject->localTransform.rotateY(rotationSpeed * timeDiff);
}

void RotatingObject::setRotationSpeed(float speed) {
	this->rotationSpeed = speed;
}

float RotatingObject::getRotationSpeed() {
	return rotationSpeed;
}

SerializableType RotatingObject::getSerializableType() {
	return SRotatingObject;
}

Json::Value RotatingObject::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["rotationSpeed"] = rotationSpeed;
	return root;
}

void RotatingObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	rotationSpeed = root["rotationSpeed"].asFloat();
}
