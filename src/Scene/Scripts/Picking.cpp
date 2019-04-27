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

Picking::Picking(GraphNode* _gameObject, const std::string& name, Camera* cam, Scene* scene)
	: Component(_gameObject, name), camera(cam), scene(scene) {

	encouragementBackground = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f),glm::vec2(720.0f, 260.0f), glm::vec2(200.0f,30.0f),Center);
    encouragement = new UiText(glm::vec2(700.0f, 260.0f), glm::vec2(60.0f,30.0f), "Press F to interact", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight);
}

void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();
	I_KEY_STATE = gameManager->getKeyState(GLFW_KEY_I);

	Collider* coll = gameObject->getComponent<Collider>();

	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getPos(), camera->getFront(), 2.0f, coll);
	if (object) {
		scene->addToRenderMap(encouragement);
		scene->addToRenderMap(encouragementBackground);
		CollectableObject *collectable = object->getComponent<CollectableObject>();
		if (collectable) {

			if (gameManager->getKeyState(GLFW_KEY_F) && collectable->getIsTaken() == false) {
				inventory.push_back(object);
				collectable->takeObject();
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
		scene->removeFromRenderMap(encouragement);
		scene->removeFromRenderMap(encouragementBackground);
	}

	if(I_KEY_STATE) {
		if (inventoryUI == true) {
			scene->removeFromRenderMap(scene->getInventoryText());
			scene->removeFromRenderMap(scene->getInventoryBackground());
			for (UiColorPlane *obj : *(scene->getObjectRepresentations())) {
				scene->removeFromRenderMap(obj);
			}

			inventoryUI = false;
		}
		else  {
			int i = 0;
			scene->addToRenderMap(scene->getInventoryText());
			scene->addToRenderMap(scene->getInventoryBackground());

			for (UiColorPlane *obj : *(scene->getObjectRepresentations())) {

				if (i >= inventory.size()) {
					break;
				}

				obj->setPosition(glm::vec2(400.0f, 400.0f)); //TODO: doesn't work :(
				scene->addToRenderMap(obj);
				i++;
			}

			inventoryUI = true;
		}
	}

	for (int i = 0; i < inventory.size(); i++) //TEMPORARY STATEMENT
	{
		CollectableObject *collectable = inventory[i]->getComponent<CollectableObject>();

		if (gameManager->getKeyState(GLFW_KEY_G) && collectable->getIsTaken() == true) {
			inventory.erase(inventory.begin() + i);
			collectable->leaveObject();
			for(UiColorPlane* obj : *(scene->getObjectRepresentations())) {
				scene->removeFromRenderMap(obj);
			}
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
