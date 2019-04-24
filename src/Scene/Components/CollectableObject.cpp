//
// Created by MB on 4/20/2019.
//

#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Render/Camera.h"
#include "CollectableObject.h"
#include "Scene/CollisionSystem.h"
#include <iostream>

CollectableObject::CollectableObject(GraphNode* _gameObject, Camera* camera):Component(_gameObject), camera(camera) {
}

void CollectableObject::takeObject()
{
    isTaken = true;
	gameObject->setActive(false);
    gameObject->getMesh()->setOpaque(false);
}

void CollectableObject::leaveObject()
{
    float distance = 2.0f;
    glm::vec2 cameraFront  =  glm::normalize(glm::vec2(camera->getFront().x, camera->getFront().z));
    glm::vec3 itemNewPosition = glm::vec3(camera->getPos().x + cameraFront.x * distance, camera->getPos().y, camera->getPos().z + cameraFront.y * distance);
    gameObject->localTransform.setPosition(itemNewPosition);
    isTaken = false;
    gameObject->setActive(true);
    gameObject->getMesh()->setOpaque(true);
}

void CollectableObject::update(float msec)
{

}

bool CollectableObject::getIsTaken() const {
    return isTaken;
}


SerializableType CollectableObject::getSerializableType() {
	return SCollectableObject;
}

Json::Value CollectableObject::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["isTaken"] = isTaken;
	root["isHitByRay"] = isHitByRay;
	root["fKeyState"] = fKeyState;
	return root;
}

void CollectableObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	isTaken = root["isTaken"].asBool();
	isHitByRay = root["isHitByRay"].asBool();
	fKeyState = root["fKeyState"].asBool();
}


CollectableObject::~CollectableObject() {}

