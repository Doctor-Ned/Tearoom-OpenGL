//
// Created by MB on 4/13/2019.
//

#include "PhysicalObject.h"
#include "Scene/GraphNode.h"
#include "Scene/CollisionSystem.h"
#include <iostream>
#include "BoxCollider.h"

bool PhysicalObject::isActive() {
	return gameObject->isActive();
}

PhysicalObject::PhysicalObject(GraphNode* _gameObject)
:Component(_gameObject) {}

void PhysicalObject::update(float msec)
{
	//GraphNode* floor = CollisionSystem::getInstance()->castRay(gameObject->worldTransform.getPosition() + glm::vec3(0.0f, -0.6f, 0.0f), glm::vec3(0,-1,0), 0.2f);
	//if(floor == nullptr)
	//{
	if(lastKnownPosition.empty())
	{
		lastKnownPosition.push(gameObject->localTransform.getPosition());
		
	}
	else
	{
		glm::vec3 norm;
		distance = glm::distance(gameObject->localTransform.getPosition(), lastKnownPosition.front());
		if(distance == 0.0f)
		{
			norm = glm::vec3(0);
		}
		else
		{
			norm = glm::normalize(gameObject->localTransform.getPosition() - lastKnownPosition.front());
		}
		direction = norm;
		//do sth with last position;
		//std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;
		lastKnownPosition.pop();
	}
	

	while(!translations.empty())
	{
		gameObject->localTransform.translate(translations.front());
		translations.pop();
	}
	if(gravity)
		gameObject->localTransform.translate(glm::vec3(0.0f, -5.0f * msec, 0.0f));

	lastKnownPosition.push(gameObject->localTransform.getPosition());
} 

bool PhysicalObject::castRayDown() {
    return false;
}

void PhysicalObject::pushTranslation(glm::vec3 translation)
{
	translations.push(translation);
}

PhysicalObject::~PhysicalObject() {

}

void PhysicalObject::updateDrawData() {}
void PhysicalObject::drawSelf(Shader *shader) {
    if(isFalling) {}
}
ShaderType PhysicalObject::getShaderType() {
    return STTexture;
}
bool PhysicalObject::isOpaque() {return mesh == nullptr || mesh->isOpaque();}
