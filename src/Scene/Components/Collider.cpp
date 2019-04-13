#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GameManager.h"
#include <iostream>
#include "Scene/GraphNode.h"

void Collider::update(float m_sec) {
	glm::vec3 gameObjectPos = gameObject->worldTransform.getPosition();

	glm::mat4 tmp(1);
	tmp[3] = glm::vec4(positionOffset, 1.0f);
	mat = gameObject->worldTransform.getMatrix() * tmp;
	position.x = mat[3][0];
	position.y = mat[3][1];
	position.z = mat[3][2];
	//std::cout << data.x << " " << data.y << " " << data.z << " " << data.w << std::endl;
	//std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;
}

void Collider::updateDrawData() {
	matrixWithoutRotation[3] = mat[3];      //notice that this will only copy the translation, but not scale. Guess we'll need to change it so that the only thing that is removed is the rotation
}

void Collider::drawSelf(Shader* shader) {
	mesh_ptr->draw(shader, matrixWithoutRotation);
}

glm::vec3 Collider::getPosition()
{
	return position;
}

ShaderType Collider::getShaderType() {
	return mesh_ptr->getShaderType();
}

void Collider::setCollisionCallback(std::function<int(Collider*)> f) {
	callbackFunctions.push_back(f);
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, glm::vec3 position)
: Component(_gameObject), type(_type), positionOffset(position)
{

}

Collider::~Collider() {

}

ColliderType Collider::getType() {
	return type;
}

std::vector<std::function<int(Collider*)>> Collider::getCallbackFunctions() {
	return callbackFunctions;
}

void Collider::setPosition(glm::vec3 pos)
{
	positionOffset = pos;
}
