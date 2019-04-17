#include "Scene/Components/PlayerMovement.h"
#include "Scene/GraphNode.h"
#include "Scene/CollisionSystem.h"
#include "Render/Camera.h"
#include <iostream>
#include "Scene/Scenes/TestScene.h"
#include "Scene/Scenes/Scene.h"
#include "glm/glm.hpp"

PlayerMovement::PlayerMovement(GraphNode* _gameObject, Camera* _camera, Scene* _scene) : Component(_gameObject), camera(_camera), scene(_scene)
{

}

void PlayerMovement::update(float msec)
{
	GraphNode* floor = CollisionSystem::getInstance()->castRay(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, -0.6f, 0.0f), glm::vec3(0,-1,0), 0.2f);
	if(floor == nullptr)
	{
		gameObject->localTransform.translate(glm::vec3(0.0f, -5.0f * msec, 0.0f));
	}

	glm::vec3 front = camera->getFront();
	glm::vec2 xz = glm::vec2(front.x, front.z);
	xz = glm::normalize(xz);
	glm::vec3 camRight = glm::normalize(glm::cross(camera->getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::vec2 right = glm::normalize(glm::vec2(camRight.x, camRight.z));

	static float speed = 2.0f;
	if(scene->getKeyState(KEY_FAST))
	{
		speed = 4.0f;
	}
	else
	{
		speed = 2.0f;
	}
	if (scene->getKeyState(KEY_FORWARD)) {
		gameObject->localTransform.translate(glm::vec3(xz.x, 0, xz.y) * msec * speed);
	}
	if (scene->getKeyState(KEY_BACKWARD)) {
		gameObject->localTransform.translate(-glm::vec3(xz.x, 0, xz.y) * msec * speed);
	}
	if (scene->getKeyState(KEY_LEFT)) {
		gameObject->localTransform.translate(-glm::vec3(right.x, 0, right.y) * msec * speed);
	}
	if (scene->getKeyState(KEY_RIGHT)) {
		gameObject->localTransform.translate(glm::vec3(right.x, 0, right.y) * msec * speed);
	}
	camera->setPos(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, 0.5f, 0.0f));
}
