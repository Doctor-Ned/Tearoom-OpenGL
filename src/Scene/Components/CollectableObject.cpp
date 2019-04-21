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


CollectableObject::~CollectableObject() {}