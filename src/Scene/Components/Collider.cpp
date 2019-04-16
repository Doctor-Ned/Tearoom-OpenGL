#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"

void Collider::update(float m_sec) {

}

void Collider::updateDrawData() {
	matrix = translate(glm::mat4(1.0f), glm::vec3(gameObject->worldTransform.getMatrix() * translate(glm::mat4(1.0f), positionOffset)[3]));
}

void Collider::drawSelf(Shader* shader) {
	mesh_ptr->drawSelf(shader, matrix);
}

ShaderType Collider::getShaderType() {
	return mesh_ptr->getShaderType();
}

void Collider::setCollisionCallback(std::function<int(Collider*)> f) {
	callbackFunctions.push_back(f);
}

bool Collider::isActive() {
	return gameObject->isActive();
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, Collision classification, bool _isTrigger, glm::vec3 positionOffset)
	: Component(_gameObject), type(_type), collisionType(classification), isTrigger(_isTrigger), positionOffset(positionOffset) { }

bool Collider::isOpaque() {
	return mesh_ptr == nullptr || mesh_ptr->isOpaque();
}

Collider::~Collider() {

}

glm::vec3 Collider::getPosition() {
	return matrix[3];
}

ColliderType Collider::getType() {
	return type;
}

Collision Collider::getCollisionType()
{
	return collisionType;
}

bool Collider::getIsTrigger()
{
	return isTrigger;
}

void Collider::setMatrix(glm::mat4 mat)
{
	matrix = mat;
}

void Collider::setIsTrigger(bool _isTrigger)
{
	isTrigger = _isTrigger;
}

std::vector<std::function<int(Collider*)>> Collider::getCallbackFunctions() {
	return callbackFunctions;
}

void Collider::setPositionOffset(glm::vec3 positionOffset) {
	this->positionOffset = positionOffset;
}
