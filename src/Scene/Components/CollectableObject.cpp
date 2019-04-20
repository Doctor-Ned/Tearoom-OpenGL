//
// Created by MB on 4/20/2019.
//

#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Mesh/MeshColorBox.h"
#include "CollectableObject.h"
#include "Collider.h"
#include <iostream>

CollectableObject::CollectableObject(GraphNode* _gameObject, Scene* _scene):Component(_gameObject), scene(_scene) {
}

void CollectableObject::takeObject()
{
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
    GameManager* gameManager = GameManager::getInstance();

    fKeyState = gameManager->getKeyState(GLFW_KEY_F);

    if(gameObject->getHitByRay() && fKeyState)
    {
        if(!isTaken)
        {
            takeObject();
        }
    }
}

CollectableObject::~CollectableObject() {}