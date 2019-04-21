#include "Scene/Components/Picking.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Scene/CollisionSystem.h"
#include "Scene/GameManager.h"
#include "Render/Camera.h"
#include "Scene/GraphNode.h"
#include "CollectableObject.h"

Picking::Picking(GraphNode* _gameObject, const std::string& name, Camera* cam)
	: Component(_gameObject, name), camera(cam)
{
}

void Picking::update(float msec)
{
	GameManager* gameManager = GameManager::getInstance();
	GraphNode* object = CollisionSystem::getInstance()->castRay(camera->getPos() + camera->getFront() * 1.5f, camera->getFront(), 3.0f);
	if (object)
	{
		CollectableObject* collectable = object->getComponent<CollectableObject>();
		if (collectable)
		{
			std::cout << "I hit the collectable object." << std::endl;

			if (gameManager->getKeyState(GLFW_KEY_F))
			{
				collectable->takeObject();
			}
		}
	}
}

Picking::~Picking()
{
}
