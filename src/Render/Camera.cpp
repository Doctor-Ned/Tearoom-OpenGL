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

glm::vec3 Camera::getRight() {
	return normalize(cross(cameraFront, cameraUp));
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

glm::vec3 Camera::getActualUp() {
	return normalize(cross(cross(cameraFront, cameraUp), cameraFront));
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
		cameraPos += getRight() * timeDelta * speed * (float)steps;
		dirty = true;
	}
}

void Camera::moveLeft(float timeDelta, int steps) {
	moveRight(timeDelta, -steps);
}

void Camera::moveUp(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += getActualUp() * timeDelta * speed * (float)steps;
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
	float k = 0.5f;
	std::shared_ptr<OctreeNode> octree = OctreeNode::getInstance();
	while (glm::distance(startPos, currentPos) < distance)
	{
		currentPos = startPos + direction * k;
		k += 0.01f;

		GraphNode* detectedNode = OctreeNode::findObjectByRayPoint(currentPos, octree);
		if (detectedNode != nullptr) {
			return detectedNode;
		}
	}
	return nullptr;
}

void Camera::recalculateFront() {
	cameraFront = normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
	                                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}
