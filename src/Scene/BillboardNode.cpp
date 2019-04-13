#include "BillboardNode.h"
#include "Render/Camera.h"

BillboardNode::BillboardNode(Camera* camera, Mesh* mesh, GraphNode* parent, bool rescale) : GraphNode(mesh, parent) {
	this->camera = camera;
}

void BillboardNode::setCamera(Camera* camera) {
	this->camera = camera;
}

void BillboardNode::update(double timeDiff) {
	GraphNode::update(timeDiff);
	recalculateMatrix();
}

Camera* BillboardNode::getCamera() {
	return camera;
}

void BillboardNode::setRescaling(bool rescale) {
	this->rescale = rescale;
}

bool BillboardNode::getRescaling() {
	return rescale;
}

void BillboardNode::recalculateMatrix() {
	glm::vec3 pos = worldTransform.getMatrix()[3] + localTransform.getMatrix()[3];
	glm::mat4 world = translate(glm::mat4(1.0f), pos);
	if (rescale) {
		float scl = length(pos - camera->getPos());
		world = scale(world, glm::vec3(scl, scl, scl));
	}
	glm::mat4 rotation = lookAt(glm::vec3(0.0f,0.0f,0.0f), -camera->getFront(), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = inverse(rotation);
	rotation[2][0] = -rotation[2][0];
	rotation[2][1] = -rotation[2][1];
	rotation[2][2] = -rotation[2][2];
	rotation[0][0] = -rotation[0][0];
	rotation[0][1] = -rotation[0][1];
	rotation[0][2] = -rotation[0][2];
	world *= rotation;
	worldTransform.setMatrix(world);
}

void BillboardNode::updateWorld() {
	GraphNode::updateWorld();
	if (dirty) {
		recalculateMatrix();
	}
}
