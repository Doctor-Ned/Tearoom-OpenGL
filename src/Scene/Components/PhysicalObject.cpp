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
		gameObject->localTransform.translate(glm::vec3(0.0f, -5.0f * msec, 0.0f));
	//}
} 

bool PhysicalObject::castRayDown() {
    return false;
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
