#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/SceneManager.h"
#include <iostream>

void Collider::update()
{
	glm::vec3 gameObjectPos = gameObject->worldTransform.getPosition();
	data.x = gameObjectPos.x + positionOffset.x;
	data.y = gameObjectPos.y + positionOffset.y;
	data.z = gameObjectPos.z + positionOffset.z;

	std::cout << data.x << " " << data.y << " " << data.z << std::endl;
}

void Collider::draw()
{
	//colliderMesh->draw(SceneManager::getInstance()->getShader(STColor), gameObject->worldTransform.Matrix(), 1.0f);
	colliderMesh->draw(gameObject->worldTransform.Matrix());
}

bool Collider::checkCollision(Collider* collider)
{
	if (type == collider->type)
	{
		float distanceSquared =
			pow(collider->data.x - data.x, 2) +
			pow(collider->data.y - data.y, 2); +
			pow(collider->data.z - data.z, 2);
		float radiusSumSquared = pow(data.w + collider->data.w , 2);
		if (distanceSquared <= radiusSumSquared)
		{
			return true;
		}
	}
	return false;
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data) 
: Component(_gameObject), type(_type), positionOffset(_data)
{
	if(type == SphereCollider)
	{
		colliderMesh = new MeshColorSphere(1.0f, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	if(type == BoxCollider)
	{
		colliderMesh = new MeshColorBox(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	colliderMesh->setUseLight(false);
	colliderMesh->setRenderMode(GL_LINES);
}

Collider::~Collider()
{
	
}
