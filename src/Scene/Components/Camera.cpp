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

Camera::Camera(GraphNode* gameObject) : Component(gameObject, "Camera") {}

glm::mat4 Camera::getView() {
	return view;
}

glm::mat4 Camera::getUntranslatedView() {
	return glm::mat4(glm::mat3(getView()));
}

void Camera::updateWorld() {
	glm::vec3 pos = gameObject->worldTransform.getPosition();
	view = lookAt(pos, pos + gameObject->getFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f));
}

Frustum Camera::getFrustum() {
	return frustum;
}

GraphNode* Camera::castRayFromCamera(glm::vec3 _direction, float distance, Collider* toIgnore) {
	return CollisionSystem::getInstance()->castRay(gameObject->worldTransform.getPosition(), _direction, distance, toIgnore);
}

void Camera::RecalculateFrustum() {
	const float nearDistance = 0.1f;
	const float farDistance = 100.0f;
	float screenRatio = GameManager::getInstance()->getScreenWidth() / GameManager::getInstance()->getScreenHeight();

	//float tangens = 2 * glm::tan(M_PI / 4);
	float tangens = glm::tan((M_PI / 180.0f) * GameManager::getInstance()->getFOV() * 0.5f);

	float nearHeight = tangens * nearDistance;
	float nearWidth = nearHeight * screenRatio;

	float farHeight = tangens * farDistance;
	float farWidth = farHeight * screenRatio;

	glm::vec3 cameraPos = gameObject->worldTransform.getPosition();
	glm::vec3 cameraFront = gameObject->getFrontVector();
	glm::vec3 nearCenter = cameraPos + cameraFront * nearDistance;
	glm::vec3 farCenter = cameraPos + cameraFront * farDistance;

	glm::vec3 actualUp = gameObject->getUpVector();
	glm::vec3 actualRight = gameObject->getRightVector();
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
	Json::Value root = Component::serialize(serializer);
	root["view"] = DataSerializer::serializeMat4(view);
	return root;
}

void Camera::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	view = DataSerializer::deserializeMat4(root["view"]);
}
