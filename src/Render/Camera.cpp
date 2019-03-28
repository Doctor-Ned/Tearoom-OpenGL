#include "Camera.h"
#include <glm/gtx/matrix_decompose.hpp>

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float speed, float rotSpeed)
	: speed(speed), rotSpeed(rotSpeed), cameraPos(cameraPos), cameraFront(cameraFront), cameraUp(cameraUp) {
	yaw = -90.0f;
	pitch = 0.0f;
	dirty = true;
}

glm::mat4 Camera::getView() {
	if (dirty) {
		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
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

void Camera::recalculateFront() {
	cameraFront = normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
	                                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}
