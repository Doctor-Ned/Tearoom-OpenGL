#include "RotatingNode.h"

RotatingNode::RotatingNode(float rotationSpeed, Mesh* mesh, GraphNode* parent) : GraphNode(mesh, parent),
                                                                                 rotationSpeed(rotationSpeed) {}

void RotatingNode::update(double timeDiff) {
	//setLocal(rotate(local, (float)(rotationSpeed * timeDiff), glm::vec3(0.0f, 1.0f, 0.0f)));
	localTransform.RotateByRadians(float((rotationSpeed * timeDiff)), glm::vec3(0.0f, 1.0f, 0.0f));
	GraphNode::update(timeDiff);
}

void RotatingNode::setRotationSpeed(float speed) {
	this->rotationSpeed = speed;
}
