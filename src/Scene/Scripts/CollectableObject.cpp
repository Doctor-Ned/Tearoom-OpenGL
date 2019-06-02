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

CollectableObject::CollectableObject(GraphNode* _gameObject):Component(_gameObject) {
}

CollectableObject::CollectableObject(GraphNode* _gameObject, ItemType i_type, UiPlane* icon, std::string desc, UiPlane* preview)
:Component(_gameObject), i_type(i_type), icon(icon) {
    desctext = new UiText(glm::vec2(100.0f, 30.0f), glm::vec2(60.0f, 30.0f), desc, glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
    if(i_type == Letter || i_type == Photo) {
        this->preview = preview;
    }
}

CollectableObject::CollectableObject(GraphNode* _gameObject, ItemType i_type, UiPlane* icon, std::string desc, UiPlane* preview, int doorID)
:Component(_gameObject), i_type(i_type), icon(icon), doorID(doorID) {
    desctext = new UiText(glm::vec2(100.0f, 30.0f), glm::vec2(60.0f, 30.0f), desc, glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
    if(i_type == Letter || i_type == Photo) {
        this->preview = preview;
    }
}

void CollectableObject::takeObject()
{
    isTaken = true;
	gameObject->setActive(false);
    gameObject->getMesh()->setOpaque(false);
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
	return root;
}

void CollectableObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	isTaken = root["isTaken"].asBool();
	isHitByRay = root["isHitByRay"].asBool();
	fKeyState = root["fKeyState"].asBool();
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

UiPlane* CollectableObject::getPreview(){
    return preview;
}

int CollectableObject::getDoorID() {
    return doorID;
}