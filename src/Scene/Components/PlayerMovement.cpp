#include "Scene/Components/PlayerMovement.h"
#include "Scene/GraphNode.h"
#include "Scene/CollisionSystem.h"
#include "Render/Camera.h"
#include <iostream>
#include "Scene/Scenes/TestScene.h"
#include "Scene/Scenes/Scene.h"
#include "glm/glm.hpp"
#include "Serialization/Serializer.h"

SerializableType PlayerMovement::getSerializableType() {
	return SPlayerMovement;
}

Json::Value PlayerMovement::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["camera"] = serializer->serialize(camera);
	root["scene"] = serializer->serialize(scene);
	return root;
}

void PlayerMovement::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
}

PlayerMovement::PlayerMovement(GraphNode* _gameObject, Camera* _camera, Scene* _scene) : Component(_gameObject, "Player movement"), camera(_camera), scene(_scene) {

}

void PlayerMovement::update(float msec) {
	GraphNode* floor = CollisionSystem::getInstance()->castRay(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, -0.6f, 0.0f), glm::vec3(0, -1, 0), 0.2f);
	if (floor == nullptr) {
		gameObject->localTransform.translate(glm::vec3(0.0f, -5.0f * msec, 0.0f));
	}

	glm::vec3 front = camera->getFront();
	glm::vec2 xz = glm::vec2(front.x, front.z);
	xz = glm::normalize(xz);
	glm::vec3 camRight = camera->getRight();

	glm::vec2 right = glm::normalize(glm::vec2(camRight.x, camRight.z));

	static float speed = 2.0f;
	GameManager* gameManager = GameManager::getInstance();
	if (gameManager->getKeyState(KEY_FAST)) {
		speed = 4.0f;
	} else {
		speed = 2.0f;
	}

	glm::vec3 direction = glm::vec3(0);
	if (gameManager->getKeyState(KEY_FORWARD)) {
		direction += glm::vec3(xz.x, 0, xz.y);
	}
	if (gameManager->getKeyState(KEY_BACKWARD)) {
		direction -= glm::vec3(xz.x, 0, xz.y);
	}
	if (gameManager->getKeyState(KEY_LEFT)) {
		direction -= glm::vec3(right.x, 0, right.y);
	}
	if (gameManager->getKeyState(KEY_RIGHT)) {
		direction += glm::vec3(right.x, 0, right.y);
	}
	if (direction != glm::vec3(0))
		direction = glm::normalize(direction);
	gameObject->localTransform.translate(direction * msec * speed);

	camera->setPos(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, 0.5f, 0.0f));
}
