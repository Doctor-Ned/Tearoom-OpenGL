#include "Billboard.h"
#include "Render/Camera.h"

Billboard::Billboard(Camera* camera, GraphNode* gameObject, bool rescale) : Component(gameObject, "Billboard") {
	this->camera = camera;
}

void Billboard::setCamera(Camera* camera) {
	this->camera = camera;
}

void Billboard::update(float msec) {
	recalculateMatrix();
}

void Billboard::updateWorld() {
	recalculateMatrix();
}

Camera* Billboard::getCamera() {
	return camera;
}

void Billboard::setRescaling(bool rescale) {
	this->rescale = rescale;
}

bool Billboard::getRescaling() {
	return rescale;
}

void Billboard::recalculateMatrix() {
	glm::vec3 pos = gameObject->worldTransform.getMatrix()[3] + gameObject->localTransform.getMatrix()[3];
	glm::mat4 world = translate(glm::mat4(1.0f), pos);
	if (rescale) {
		glm::vec3 right = camera->getRight();
		glm::vec3 up = camera->getActualUp();
		glm::vec4 plane1 = Global::planeEquationOfPoints(pos, pos + right, pos + up);
		glm::vec3 camPos = camera->getPos();
		glm::vec4 plane2 = Global::planeEquationOfPoints(camPos, camPos + right, camPos + up);
		float scl = Global::distanceBetweenParallelPlanes(plane1, plane2);
		world = scale(world, glm::vec3(scl, scl, scl));
	}
	glm::mat4 rotation = lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -camera->getFront(), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = inverse(rotation);
	rotation[2][0] = -rotation[2][0];
	rotation[2][1] = -rotation[2][1];
	rotation[2][2] = -rotation[2][2];
	rotation[0][0] = -rotation[0][0];
	rotation[0][1] = -rotation[0][1];
	rotation[0][2] = -rotation[0][2];
	world *= rotation;
	gameObject->worldTransform.setMatrix(world);
}
