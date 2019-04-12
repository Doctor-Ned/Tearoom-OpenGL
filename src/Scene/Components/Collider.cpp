#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GameManager.h"
#include <iostream>
#include "Scene/GraphNode.h"

void Collider::update(float m_sec) {
	glm::vec3 gameObjectPos = gameObject->worldTransform.getPosition();

	glm::mat4 tmp(1);
	tmp[3][0] = positionOffset.x;
	tmp[3][1] = positionOffset.y;
	tmp[3][2] = positionOffset.z;
	mat = gameObject->worldTransform.Matrix() * tmp;
	data.x = mat[3][0];
	data.y = mat[3][1];
	data.z = mat[3][2];
	//std::cout << data.x << " " << data.y << " " << data.z << " " << data.w << std::endl;
	//std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;
}

void Collider::updateDrawData() {
	matrixWithoutRotation[3] = mat[3];      //notice that this will only copy the translation, but not scale. Guess we'll need to change it so that the only thing that is removed is the rotation
}

void Collider::drawSelf(Shader* shader) {
	colliderMesh->draw(shader, matrixWithoutRotation);
}

ShaderType Collider::getShaderType() {
	return colliderMesh->getShaderType();
}

void Collider::SetCollisionCallback(std::function<int(Collider*)> f) {
	callbackFunctions.push_back(f);
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data)
	: Component(_gameObject), type(_type), positionOffset(_data), data(_data), matrixWithoutRotation(glm::mat4(1.0f)) {
	if (type == SphereCollider) {
		colliderMesh = new MeshColorSphere(data.w, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	if (type == BoxCollider) {
		colliderMesh = new MeshColorBox(glm::vec3(-data.w, -data.w, -data.w), glm::vec3(data.w, data.w, data.w), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	colliderMesh->setUseLight(false);
	colliderMesh->setRenderMode(GL_LINES);

	mat = glm::mat4(1);
}

Collider::~Collider() {

}

glm::vec4 Collider::getData() {
	return data;
}

ColliderType Collider::getType() {
	return type;
}

std::vector<std::function<int(Collider*)>> Collider::getCallbackFunctions() {
	return callbackFunctions;
}
