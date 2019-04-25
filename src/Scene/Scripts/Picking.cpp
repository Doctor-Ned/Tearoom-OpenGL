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

Picking::Picking(GraphNode* _gameObject, const std::string& name, Camera* cam, Scene* scene)
	: Component(_gameObject, name), camera(cam), scene(scene) {

	hud = new UiPlane("res/textures/inventory.png", glm::vec2(0.0f, 5.0f), glm::vec2(0.5f, 0.5f), Center);
}

void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();
	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getPos() + camera->getFront() * 1.5f,
		camera->getFront(), 1.0f);
	if (object) {
		CollectableObject *collectable = object->getComponent<CollectableObject>();
		if (collectable) {

			if (gameManager->getKeyState(GLFW_KEY_F) && collectable->getIsTaken() == false) {
				inventory.push_back(object);
				collectable->takeObject();
			}
		}
	}

	if (gameManager->getKeyState(GLFW_KEY_I && inventoryUI == false)) {
		inventoryUI = true;
		scene->addToRenderMap(hud);

	}

	if (gameManager->getKeyState(GLFW_KEY_I && inventoryUI == true)) {
		inventoryUI = false;
		scene->removeFromRenderMap(hud);
	}

	for (int i = 0; i < inventory.size(); i++) //TEMPORARY STATEMENT
	{
		CollectableObject *collectable = inventory[i]->getComponent<CollectableObject>();

		if (gameManager->getKeyState(GLFW_KEY_G) && collectable->getIsTaken() == true) {
			inventory.erase(inventory.begin() + i);
			collectable->leaveObject();
		}

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
