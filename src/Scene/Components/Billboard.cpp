#include "Billboard.h"
#include "Serialization/Serializer.h"
#include "Scene/Components/Camera.h"

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

SerializableType Billboard::getSerializableType() {
	return SBillboard;
}

Json::Value Billboard::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["camera"] = serializer->serialize(camera);
	root["rescale"] = rescale;
	return root;
}

void Billboard::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
	rescale = root["rescale"].asBool();
}

void Billboard::renderGui() {
	Component::renderGui();
	if(active) {
		bool rescale = getRescaling();
		ImGui::Checkbox("Rescale", &rescale);
		if(rescale != getRescaling()) {
			setRescaling(rescale);
		}
	}
}

void Billboard::recalculateMatrix() {
	glm::vec3 pos = gameObject->worldTransform.getMatrix()[3] + gameObject->localTransform.getMatrix()[3];
	glm::mat4 world = translate(glm::mat4(1.0f), pos);
	if (rescale) {
		glm::vec3 right = camera->getGameObject()->getRightVector();
		glm::vec3 up = camera->getGameObject()->getUpVector();
		glm::vec4 plane1 = Global::planeEquationOfPoints(pos, pos + right, pos + up);
		glm::vec3 camPos = camera->getGameObject()->getPosition();
		glm::vec4 plane2 = Global::planeEquationOfPoints(camPos, camPos + right, camPos + up);
		float scl = Global::distanceBetweenParallelPlanes(plane1, plane2);
		world = scale(world, glm::vec3(scl, scl, scl));
	}
	glm::mat4 rotation = lookAt(glm::vec3(0.0f, 0.0f, 0.0f), camera->getGameObject()->getBackVector(), glm::vec3(0.0f, 1.0f, 0.0f));
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
