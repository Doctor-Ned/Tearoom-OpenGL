#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GameManager.h"
#include <iostream>
#include "Scene/GraphNode.h"

void Collider::update(float m_sec) {
	
}

void Collider::updateDrawData() {
	matrix = translate(glm::mat4(1.0f), glm::vec3(gameObject->worldTransform.getMatrix() * translate(glm::mat4(1.0f), positionOffset)[3]));
}

void Collider::drawSelf(Shader* shader) {
	mesh_ptr->draw(shader, matrix);
}

ShaderType Collider::getShaderType() {
	return mesh_ptr->getShaderType();
}

void Collider::setCollisionCallback(std::function<int(Collider*)> f) {
	callbackFunctions.push_back(f);
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, glm::vec3 positionOffset)
	: Component(_gameObject), type(_type), positionOffset(positionOffset) { }

Collider::~Collider() {

}

glm::vec3 Collider::getPosition() {
	return matrix[3];
}

ColliderType Collider::getType() {
	return type;
}

std::vector<std::function<int(Collider*)>> Collider::getCallbackFunctions() {
	return callbackFunctions;
}

void Collider::setPositionOffset(glm::vec3 positionOffset) {
	this->positionOffset = positionOffset;
}
