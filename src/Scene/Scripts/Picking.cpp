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

}

void showEncouragement() {

}

void Picking::update(float msec) {
	GameManager *gameManager = GameManager::getInstance();
	I_KEY_STATE = gameManager->getKeyState(GLFW_KEY_I);

	Collider* coll = gameObject->getComponent<Collider>();

	GraphNode * object = CollisionSystem::getInstance()->castRay(camera->getPos(), camera->getFront(), 2.0f, coll);
	if (object) {
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

	if(I_KEY_STATE && inventoryUI == true){
		scene->getInventoryBackground()->setActive(false);
		scene->getInventoryBackground()->setOpacity(0.0f);
		scene->getUiRoot()->setOpacity(0.0f);
		scene->getInventoryText()->setOpacity(0.0f);
		scene->addToRenderMap(scene->getInventoryText());
		inventoryUI = false;
	}

	if(I_KEY_STATE && inventoryUI == false) {
				scene->getInventoryBackground()->setActive(true);
				scene->getInventoryBackground()->setOpacity(1.0f);
				int i=0;
				scene->addToRenderMap(scene->getInventoryText());

				for(UiColorPlane* obj : *(scene->getObjectRepresentations())) {

					if(i >= inventory.size()) {
						break;
					}

					obj->setPosition(glm::vec2(400.0f, 400.0f)); //TODO: doesn't work :(
					obj->setOpacity(1.0f);
					i++;
					std::cout<<i<<std::endl;
				}

				inventoryUI = true;
		}


	for (int i = 0; i < inventory.size(); i++) //TEMPORARY STATEMENT
	{
		CollectableObject *collectable = inventory[i]->getComponent<CollectableObject>();

		if (gameManager->getKeyState(GLFW_KEY_G) && collectable->getIsTaken() == true) {
			inventory.erase(inventory.begin() + i);
			collectable->leaveObject();
			for(UiColorPlane* obj : *(scene->getObjectRepresentations())) {
				obj->setOpacity(0.0f);
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
