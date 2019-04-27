#include "Camera.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include "Scene/CollisionSystem.h"
#include "Scene/OctreeNode.h"
#include "Scene/GraphNode.h"
#include "Scene/Components/Collider.h"
#include <algorithm>
#include "Scene/Components/AnimationController.h"
#include "Serialization/DataSerializer.h"

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
		cameraPos += cameraFront * timeDelta * speed * static_cast<float>(steps);
		dirty = true;
	}
}

void Camera::moveBackward(float timeDelta, int steps) {
	moveForward(timeDelta, -steps);
}

void Camera::moveRight(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += getRight() * timeDelta * speed * static_cast<float>(steps);
		dirty = true;
	}
}

void Camera::moveLeft(float timeDelta, int steps) {
	moveRight(timeDelta, -steps);
}

void Camera::moveUp(float timeDelta, int steps) {
	if (timeDelta != 0.0f) {
		cameraPos += getActualUp() * timeDelta * speed * static_cast<float>(steps);
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

GraphNode* Camera::castRayFromCamera(glm::vec3 _direction, float distance, Collider* toIgnore)
{
	return CollisionSystem::getInstance()->castRay(this->cameraPos, _direction, distance, toIgnore);
}

void Camera::RecalculateFrustum()
{
	float nearDistance = 0.1f;
	float farDistance = 100.0f;
	float screenRatio = GameManager::getInstance()->getScreenWidth() / GameManager::getInstance()->getScreenHeight();

	//float tangens = 2 * glm::tan(M_PI / 4);
	float tangens = glm::tan((M_PI / 180.0f) * 45.0f * 0.5f);
	
	float nearHeight = tangens * nearDistance;
	float nearWidth = nearHeight * screenRatio;

	float farHeight = tangens * farDistance;
	float farWidth = farHeight * screenRatio;

	glm::vec3 nearCenter = cameraPos + cameraFront * nearDistance;
	glm::vec3 farCenter = cameraPos + cameraFront * farDistance;

	glm::vec3 actualUp = getActualUp();
	glm::vec3 actualRight = normalize(cross(cameraFront, actualUp));
	//4 corners of the near plane of frustum 
	frustum.nearTopLeft = nearCenter + (actualUp * nearHeight) - (actualRight * nearWidth);
	frustum.nearTopRight = nearCenter + (actualUp * nearHeight) + (actualRight * nearWidth);
	frustum.nearBottomLeft = nearCenter - (actualUp * nearHeight) - (actualRight * nearWidth);
	frustum.nearBottomRight = nearCenter - (actualUp * nearHeight) + (actualRight * nearWidth);
	//4 corners of the far plane of frustum 
	frustum.farTopLeft = farCenter + (actualUp * farHeight) - (actualRight * farWidth);
	frustum.farTopRight = farCenter + (actualUp * farHeight) + (actualRight * farWidth);
	frustum.farBottomLeft = farCenter - (actualUp * farHeight) - (actualRight * farWidth);
	frustum.farBottomRight = farCenter - (actualUp * farHeight) + (actualRight * farWidth);

	frustum.planes[TOP] = { frustum.nearTopRight, frustum.nearTopLeft, frustum.farTopLeft };
	frustum.planes[BOTTOM] = { frustum.nearBottomLeft, frustum.nearBottomRight, frustum.farBottomRight };
	frustum.planes[LEFT] = { frustum.nearTopLeft, frustum.nearBottomLeft, frustum.farBottomLeft };
	frustum.planes[RIGHT] = { frustum.nearBottomRight, frustum.nearTopRight, frustum.farBottomRight };
	frustum.planes[NEARP] = { frustum.nearTopLeft, frustum.nearTopRight, frustum.nearBottomRight };
	frustum.planes[FARP] = { frustum.farTopRight, frustum.farTopLeft, frustum.farBottomLeft };

}

SerializableType Camera::getSerializableType() {
	return SCamera;
}

Json::Value Camera::serialize(Serializer* serializer) {
	Json::Value root;
	root["speed"] = speed;
	root["rotSpeed"] = rotSpeed;
	root["yaw"] = yaw;
	root["pitch"] = pitch;
	root["view"] = DataSerializer::serializeMat4(view);
	root["cameraPos"] = DataSerializer::serializeVec3(cameraPos);
	root["cameraFront"] = DataSerializer::serializeVec3(cameraFront);
	root["cameraUp"] = DataSerializer::serializeVec3(cameraUp);
	return root;
}

void Camera::deserialize(Json::Value& root, Serializer* serializer) {
	speed = root["speed"].asFloat();
	rotSpeed = root["rotSpeed"].asFloat();
	yaw = root["yaw"].asFloat();
	pitch = root["pitch"].asFloat();
	view = DataSerializer::deserializeMat4(root["view"]);
	cameraPos = DataSerializer::deserializeVec3(root["cameraPos"]);
	cameraFront = DataSerializer::deserializeVec3(root["cameraFront"]);
	cameraUp = DataSerializer::deserializeVec3(root["cameraUp"]);
	dirty = true;
}

void Camera::recalculateFront() {
	cameraFront = normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
	                                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}
