#include "Camera.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include "Scene/CollisionSystem.h"
#include "Scene/OctreeNode.h"
#include "Scene/GraphNode.h"
#include "Scene/Components/Collider.h"

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float speed, float rotSpeed)
	: speed(speed), rotSpeed(rotSpeed), cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp) {
	yaw = -90.0f;
	pitch = 0.0f;
	dirty = true;
}

glm::mat4 Camera::getView() {
	if (dirty) {
		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		dirty = false;
	}
	return view;
}

glm::mat4 Camera::getUntranslatedView() {
	return glm::mat4(glm::mat3(getView()));
}

glm::vec3 Camera::getFront() {
	return cameraFront;
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::vec3 Camera::getUp() {
	return cameraUp;
}

float Camera::getYaw() {
	return yaw;
}

float Camera::getPitch() {
	return pitch;
}

void Camera::setPos(glm::vec3 pos) {
	cameraPos = pos;
	dirty = true;
}

void Camera::rotateX(float timeDelta) {
	if (timeDelta != 0.0f) {
		yaw += timeDelta * rotSpeed;
		recalculateFront();
		dirty = true;
	}
}

void Camera::rotateY(float timeDelta) {
	if (timeDelta != 0.0f) {
		pitch += timeDelta * rotSpeed;
		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		else if (pitch < -89.0f) {
			pitch = -89.0f;
		}
		recalculateFront();
		dirty = true;
	}
}

void Camera::moveForward(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += cameraFront * timeDelta * speed * (float)steps;
		dirty = true;
	}
}

void Camera::moveBackward(float timeDelta, int steps) {
	moveForward(timeDelta, -steps);
}

void Camera::moveRight(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += normalize(cross(cameraFront, cameraUp)) * timeDelta * speed * (float)steps;
		dirty = true;
	}
}

void Camera::moveLeft(float timeDelta, int steps) {
	moveRight(timeDelta, -steps);
}

void Camera::moveUp(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += normalize(cross(cross(cameraFront, cameraUp), cameraFront)) * timeDelta * speed * (float)steps;
		dirty = true;
	}
}

void Camera::moveDown(float timeDelta, int steps) {
	moveUp(timeDelta, -steps);
}

void Camera::setSpeed(float speed) {
	this->speed = speed;
}

void Camera::setRotSpeed(float rotSpeed) {
	this->rotSpeed = rotSpeed;
}

GraphNode* Camera::castRayFromCamera(glm::vec3 _direction, float distance)
{
	glm::vec3 startPos = cameraPos;
	glm::vec3 direction = glm::normalize(_direction);
	glm::vec3 currentPos = startPos;
	float k = 0.01f;
	std::shared_ptr<OctreeNode> octree = OctreeNode::getInstance();
	while (glm::distance(startPos, currentPos) < distance)
	{
		currentPos = startPos + direction * k;
		k += 0.01f;
		for (GraphNode* game_object : octree->getGameObjects())
		{
			Collider* collider = game_object->getComponent<Collider>();
			if (collider != nullptr)
			{
				if (CollisionSystem::getInstance()->containTest(currentPos, collider))
				{
					std::cout << "ray hit gameobject" << std::endl;
					return game_object;
				}
			}
		}

		for (auto& node : octree->getNodes())
		{
			//check if node contain ray point
			if (OctreeNode::containTest(currentPos, node->getBox())) {
				for (GraphNode* nodeGameObject : node->getGameObjects())
				{
					Collider* collider = nodeGameObject->getComponent<Collider>();
					if (collider != nullptr)
					{
						//check if point collides with collider
						if (CollisionSystem::getInstance()->containTest(currentPos, collider))
						{
							std::cout << "ray hit gameobject" << std::endl;
							return nodeGameObject;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

void Camera::recalculateFront() {
	cameraFront = normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
	                                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}
