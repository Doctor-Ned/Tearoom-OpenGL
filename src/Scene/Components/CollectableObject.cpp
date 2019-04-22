//
// Created by MB on 4/20/2019.
//

#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Render/Camera.h"
#include "CollectableObject.h"
#include "Scene/CollisionSystem.h"
#include <iostream>

CollectableObject::CollectableObject(GraphNode* _gameObject):Component(_gameObject) {
}

void CollectableObject::takeObject()
{
	gameObject->setActive(false);
    isTaken = true;
   // std::cout<<"HAAALOOOO"<<std::endl;
    gameObject->getMesh()->setOpaque(false);
}

void CollectableObject::leaveObject()
{
    isTaken = false;
}

void CollectableObject::update(float msec)
{
	//GameManager* gameManager = GameManager::getInstance();

	/*fKeyState = gameManager->getKeyState(GLFW_KEY_F);

	if (gameObject->getHitByRay() && fKeyState)
	{
		if (!isTaken)
		{
			takeObject();
		}
	}*/
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
