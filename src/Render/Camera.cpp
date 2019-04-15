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

Frustum Camera::getFrustum()
{
	return frustum;
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
	return CollisionSystem::getInstance()->castRay(this->cameraPos, _direction, distance);
}

void Camera::RecalculateFrustum()
{
	float nearDistance = 0.1f;
	float farDistance = 100.0f;
	float screenRatio = GameManager::getInstance()->getScreenWidth() / GameManager::getInstance()->getScreenHeight();

	float tangens = 2 * glm::tan(M_PI / 4);
	float nearHeight = tangens * nearDistance;
	float nearWidth = nearHeight * screenRatio;

	float farHeight = tangens * farDistance;
	float farWidth = farHeight * screenRatio;

	glm::vec3 nearCenter = cameraPos + cameraFront * nearDistance;
	glm::vec3 farCenter = cameraPos + cameraFront * farDistance;

	glm::vec3 actualUp = getActualUp();
	glm::vec3 actualRight = normalize(cross(cameraFront, actualUp));
	//4 corners of the near plane of frustum 
	frustum.nearTopLeft = nearCenter + (actualUp * (nearHeight / 2)) - (actualRight * (nearWidth / 2));
	frustum.nearTopRight = nearCenter + (actualUp * (nearHeight / 2)) + (actualRight * (nearWidth / 2));
	frustum.nearBottomLeft = nearCenter - (actualUp * (nearHeight / 2)) - (actualRight * (nearWidth / 2));
	frustum.nearBottomRight = nearCenter - (actualUp * (nearHeight / 2)) + (actualRight * (nearWidth / 2));
	//4 corners of the far plane of frustum 
	frustum.farTopLeft = farCenter + (actualUp * (farHeight / 2)) - (actualRight * (farWidth / 2));
	frustum.farTopRight = farCenter + (actualUp * (farHeight / 2)) + (actualRight * (farWidth / 2));
	frustum.farBottomLeft = farCenter - (actualUp * (farHeight / 2)) - (actualRight * (farWidth / 2));
	frustum.farBottomRight = farCenter - (actualUp * (farHeight / 2)) + (actualRight * (farWidth / 2));

	frustum.planes[TOP] = { frustum.nearBottomLeft, frustum.nearTopLeft, frustum.farTopLeft };
	frustum.planes[BOTTOM] = { frustum.nearBottomLeft, frustum.farBottomRight, frustum.farBottomRight };
	frustum.planes[LEFT] = { frustum.nearTopLeft, frustum.nearBottomLeft, frustum.nearBottomLeft };
	frustum.planes[RIGHT] = { frustum.nearBottomRight, frustum.nearTopRight, frustum.nearBottomRight };
	frustum.planes[NEARP] = { frustum.nearTopLeft, frustum.nearTopRight, frustum.nearBottomRight };
	frustum.planes[FARP] = { frustum.farTopRight, frustum.farTopLeft, frustum.farBottomLeft };

}

void Camera::recalculateFront() {
	cameraFront = normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
	                                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}
