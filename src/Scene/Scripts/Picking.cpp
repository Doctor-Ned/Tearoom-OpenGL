#include "Scene/Scripts/Picking.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Scene/CollisionSystem.h"
#include "Scene/GameManager.h"
#include "Render/Camera.h"
#include "Scene/GraphNode.h"
#include "CollectableObject.h"
#include "Serialization/Serializer.h"
#include "Ui/UiPlane.h"
#include "Scene/Components/Animation.h"
#include "Scene/Components/AnimationController.h"

Picking::Picking(GraphNode* _gameObject, Camera* cam, Scene* scene, const std::string& name)
	: Component(_gameObject, name), camera(cam), scene(scene) {
	UiElement *root = scene->getUiRoot();
	inventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	inventoryCanvas->setParent(root);
	inventoryCanvas->setActive(false);

	UiText *inventoryText = new UiText(glm::vec2(1135.0f, 280.0f), glm::vec2(60.0f, 30.0f), "Inventory", glm::vec4(0.39f, 0.3f, 0.25f, 1.0f), MatchHeight);
	UiPlane *inventoryBackground = new UiPlane("res/textures/inventory.jpg", glm::vec2(1285.0f, 500.0f), glm::vec2(300.0f, 500.0f), Right);
	//UiPlane* hud = new UiPlane("res/textures/inventory.png", glm::vec2(500.0f, 500.0f), glm::vec2(400.0f, 400.0f), Center);
	//inventoryCanvas->addChild(hud);
	inventoryCanvas->addChild(inventoryBackground);
	inventoryCanvas->addChild(inventoryText);
	encouragementCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	encouragementCanvas->setParent(root);
	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f),glm::vec2(720.0f, 260.0f), glm::vec2(200.0f,30.0f),Center);
    encouragement = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f,30.0f), "Press F to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementCanvas->setActive(false);
	encouragementCanvas->addChild(encouragementBackground);
	encouragementCanvas->addChild(encouragement);
}

void Picking::hideInventoryUi() {
	inventoryCanvas->setActive(false);
	for(auto &obj : *(scene->getObjectRepresentations())) {
		inventoryCanvas->removeChild(obj);
	}
}

void Picking::showInventoryUi() {
	int i = 0;
	inventoryCanvas->setActive(true);

	for (UiPlane *obj : *(scene->getObjectRepresentations())) {
		if (i >= inventory.size()) {
			break;
		}
		//obj->setPosition(glm::vec2(400.0f, 400.0f)); //TODO: doesn't work :(
		inventoryCanvas->addChild(obj);
		i++;
	}
}

bool Picking::getSwitch() {
	return inventoryUI;
}

void Picking::setSwitch(bool ifShown) {
	this->inventoryUI = ifShown;
}

void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();

	Collider* coll = gameObject->getComponent<Collider>();

	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getPos(), camera->getFront(), 2.0f, coll);
	if (object) {
		encouragementCanvas->setActive(true);
		CollectableObject *collectable = object->getComponent<CollectableObject>();
		if (collectable) {

			if (gameManager->getKeyState(GLFW_KEY_F) && collectable->getIsTaken() == false) {
				inventory.push_back(object);
				collectable->takeObject();

				if(inventoryUI) //TODO: getting proper item icon to render
				{
					inventoryCanvas->addChild(scene->getObjectRepresentations()->front());
				}
			}
		}
		Animation* anim = object->getComponent<Animation>();
		if(anim)
		{
			if (gameManager->getKeyState(GLFW_KEY_F)) {
				anim->play();
			}
		}
		AnimationController* animController = object->getComponent<AnimationController>();
		if(animController)
		{
			if (gameManager->getKeyState(GLFW_KEY_F))
			{
				animController->startAnimation();
			}
		}

	}
	else {
		encouragementCanvas->setActive(false);
	}

	for (int i = 0; i < inventory.size(); i++) //TEMPORARY STATEMENT
	{
		CollectableObject *collectable = inventory[i]->getComponent<CollectableObject>();

		if (gameManager->getKeyState(GLFW_KEY_G) && collectable->getIsTaken()) {
			inventory.erase(inventory.begin() + i);
			collectable->leaveObject();
			for(UiPlane* obj : *(scene->getObjectRepresentations())) {
				inventoryCanvas->removeChild(obj);
			}
		}
	}

	if(inventoryUI)
	{
		showInventoryUi();
	}
	else
	{
		hideInventoryUi();
	}

}

Picking::~Picking() {}

SerializableType Picking::getSerializableType() {
	return SPicking;
}

Json::Value Picking::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["camera"] = serializer->serialize(camera);
	return root;
}

void Picking::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	camera = dynamic_cast<Camera*>(serializer->deserialize(root["camera"]).object);
}
