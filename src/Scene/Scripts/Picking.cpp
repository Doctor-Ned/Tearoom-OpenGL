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

Picking::Picking(GraphNode* _gameObject, Camera* camera, Scene* scene,  const std::string& name )
	: Component(_gameObject, name), scene(scene){
	UiElement *root = scene->getUiRoot();
	inventoryCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	inventoryCanvas->setParent(root);
	inventoryCanvas->setActive(false);

	photosInventory = new UiPlane("res/textures/photosChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	itemsInventory = new UiPlane("res/textures/itemsChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);
	letterInventory = new UiPlane("res/textures/lettersChosenInventory.PNG", glm::vec2(1285.0f, 580.0f), glm::vec2(390.0f, 300.0f), Right);

	//inventoryCanvas->addChild(hud);
	inventoryCanvas->addChild(photosInventory);
	inventoryCanvas->addChild(itemsInventory);
	inventoryCanvas->addChild(letterInventory);

	encouragementCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
	encouragementCanvas->setParent(root);
	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(720.0f, 260.0f), glm::vec2(200.0f, 30.0f), Center);
	encouragementPick = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press F to pick up", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementActivate = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f, 30.0f), "Press F to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
	encouragementCanvas->setActive(false);
	encouragementCanvas->addChild(encouragementBackground);
	encouragementCanvas->addChild(encouragementPick);
	encouragementCanvas->addChild(encouragementActivate);

}


void Picking::hideInventoryUi() {
	inventoryCanvas->setActive(false);
	for(auto &obj : *(scene->getObjectRepresentations())) {
		inventoryCanvas->removeChild(obj);
	}
}

void Picking::showInventoryUi() {
	int i = 0;
	Picking::inventoryCanvas->setActive(true);

	if(letterInventory->isActive()) {
		for (GraphNode* obj : inventory) {
            std::cout<<"HALO"<<std::endl;
			inventoryCanvas->addChild(obj->getComponent<CollectableObject>()->getIcon());
			i++;
		}
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
	GraphNode * object = CollisionSystem::getInstance()->castRay(gameManager->getCamera()->getPos(), gameManager->getCamera()->getFront(), 2.0f, coll);

	if (object) {

		CollectableObject *collectable = object->getComponent<CollectableObject>();
		if (collectable) {
			encouragementCanvas->setActive(true);
			encouragementActivate->setActive(false);
			encouragementPick->setActive(true);

			if (gameManager->getKeyState(GLFW_KEY_F) && collectable->getIsTaken() == false) {
				inventory.push_back(object);
				collectable->takeObject();

				if(inventoryUI) //TODO: getting proper item icon to render
				{
					int i = 0;
					for(UiPlane* icon : *scene->getObjectRepresentations())
					{
						icon->setPosition(glm::vec2(icon->getPosition().x + (i * 81), icon->getPosition().y));
						inventoryCanvas->addChild(icon);
						i++;
					}
				}
			}
		}
		Animation* anim = object->getComponent<Animation>();
		if(anim)
		{
            encouragementCanvas->setActive(true);
            encouragementActivate->setActive(true);
            encouragementPick->setActive(false);

			if (gameManager->getKeyState(GLFW_KEY_F)) {
				anim->play();
			}
		}
		AnimationController* animController = object->getComponent<AnimationController>();
		if(animController)
		{
            encouragementCanvas->setActive(true);
			encouragementActivate->setActive(true);
			encouragementPick->setActive(false);

			if (gameManager->getKeyState(GLFW_KEY_F))
			{
			    if(animController->getType() == DoorOpeningX
			    || animController->getType() == DoorOpeningY) {
                    inventoryCanvas->setActive(true);
                    animController->startAnimation();
                }
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
	//TEMPORARY______________________________
	if(gameManager->getKeyState(GLFW_KEY_P)) {
		letterInventory->setActive(false);
		itemsInventory->setActive(false);
		photosInventory->setActive(true);
	}
	if(gameManager->getKeyState(GLFW_KEY_O)) {
		letterInventory->setActive(true);
		itemsInventory->setActive(false);
		photosInventory->setActive(false);
	}
	if(gameManager->getKeyState(GLFW_KEY_I)) {
		letterInventory->setActive(false);
		itemsInventory->setActive(true);
		photosInventory->setActive(false);
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
	Scene *Picking::getScene() const {
		return scene;
	}
	UiCanvas *Picking::getInventoryCanvas() const {
		return inventoryCanvas;
	}
