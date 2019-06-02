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

CollectableObject::CollectableObject(GraphNode* _gameObject) :Component(_gameObject) {}

CollectableObject::CollectableObject(GraphNode* _gameObject, ItemType i_type, std::string icon, glm::vec2 iconPos, glm::vec2 iconSize, std::string desc, std::string preview, glm::vec2 previewPos, glm::vec2 previewSize)
	: Component(_gameObject), i_type(i_type), desc(desc), iconResource(icon), iconPos(iconPos), iconSize(iconSize), previewResource(preview), previewPos(previewPos), previewSize(previewSize) {
	initializeUiPlanes();
}

CollectableObject::CollectableObject(GraphNode* _gameObject, ItemType i_type, std::string icon, glm::vec2 iconPos, glm::vec2 iconSize, std::string desc, std::string preview, glm::vec2 previewPos, glm::vec2 previewSize, int doorID)
	: Component(_gameObject), i_type(i_type), desc(desc), iconResource(icon), iconPos(iconPos), iconSize(iconSize), previewResource(preview), previewPos(previewPos), previewSize(previewSize), doorID(doorID) {
	initializeUiPlanes();
}

void CollectableObject::takeObject() {
	isTaken = true;
	gameObject->setActive(false);
	gameObject->getMesh()->setOpaque(false);
}

void CollectableObject::update(float msec) {

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
	root["iconResource"] = iconResource;
	root["previewResource"] = previewResource;
	root["i_type"] = static_cast<int>(i_type);
	root["iconPos"] = DataSerializer::serializeVec2(iconPos);
	root["iconSize"] = DataSerializer::serializeVec2(iconSize);
	root["previewPos"] = DataSerializer::serializeVec2(previewPos);
	root["previewSize"] = DataSerializer::serializeVec2(previewSize);
	root["desc"] = desc;
	return root;
}

void CollectableObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	isTaken = root["isTaken"].asBool();
	isHitByRay = root["isHitByRay"].asBool();
	fKeyState = root["fKeyState"].asBool();
	iconResource = root["iconResource"].asString();
	previewResource = root["previewResource"].asString();
	iconPos = DataSerializer::deserializeVec2(root["iconPos"]);
	iconSize = DataSerializer::deserializeVec2(root["iconSize"]);
	previewPos = DataSerializer::deserializeVec2(root["previewPos"]);
	previewSize = DataSerializer::deserializeVec2(root["previewSize"]);
	desc = root["desc"].asString();
	i_type = static_cast<ItemType>(root.get("i_type", 0).asInt());
	initializeUiPlanes();
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

UiPlane* CollectableObject::getPreview() {
	return preview;
}

int CollectableObject::getDoorID() {
	return doorID;
}

void CollectableObject::renderGui() {
	Component::renderGui();
	if(active) {
		ImGui::DragInt("Door ID", &doorID, 1, 0, 100);
	}
}

void CollectableObject::initializeUiPlanes() {
	desctext = new UiText(glm::vec2(100.0f, 30.0f), glm::vec2(60.0f, 30.0f), desc, glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	icon = new UiPlane(iconResource.c_str(), iconPos, iconSize, Right);
	if (!previewResource.empty()) {
		preview = new UiPlane(previewResource.c_str(), previewPos, previewSize, Right);
	} else {
		preview = nullptr;
	}
}
