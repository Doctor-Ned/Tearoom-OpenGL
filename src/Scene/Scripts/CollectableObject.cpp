//
// Created by MB on 4/20/2019.
//

#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Scene/Components/Camera.h"
#include "CollectableObject.h"
#include "Scene/CollisionSystem.h"
#include <iostream>
#include "Serialization/Serializer.h"

CollectableObject::CollectableObject(GraphNode* _gameObject, Camera* camera):Component(_gameObject), camera(camera) {
}

CollectableObject::CollectableObject(GraphNode* _gameObject, Camera* camera, ItemType i_type, UiPlane* icon, std::string desc)
:Component(_gameObject), camera(camera), i_type(i_type), icon(icon) {
    desctext = new UiText(glm::vec2(1010.0f, 360.0f), glm::vec2(60.0f, 30.0f), desc, glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
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
	glm::vec3 front = camera->getGameObject()->getFrontVector();
	glm::vec3 pos = camera->getGameObject()->getPosition();
    glm::vec2 cameraFront  =  glm::normalize(glm::vec2(front.x, front.z));
    glm::vec3 itemNewPosition = glm::vec3(pos.x + cameraFront.x * distance, pos.y, pos.z + cameraFront.y * distance);
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
UiText* CollectableObject::getDescription() {
    return desctext;
}

SerializableType CollectableObject::getSerializableType() {
	return SCollectableObject;
}

Json::Value CollectableObject::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["isTaken"] = isTaken;
	root["isHitByRay"] = isHitByRay;
	root["fKeyState"] = fKeyState;
	root["camera"] = serializer->serialize(camera);
	return root;
}

void CollectableObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	isTaken = root["isTaken"].asBool();
	isHitByRay = root["isHitByRay"].asBool();
	fKeyState = root["fKeyState"].asBool();
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
}


CollectableObject::~CollectableObject() {}

ItemType CollectableObject::getI_type() const {
    return i_type;
}

UiPlane *CollectableObject::getIcon() const {
    return icon;
}

void CollectableObject::setButton(UiButton* button) {
    itemButton = button;
}

UiButton* CollectableObject::getButton() {
    return itemButton;
}
