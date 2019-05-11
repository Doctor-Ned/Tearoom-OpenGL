//
// Created by MB on 4/13/2019.
//

#include "PhysicalObject.h"
#include "Scene/GraphNode.h"
#include "Scene/CollisionSystem.h"
#include <iostream>
#include "BoxCollider.h"
#include "Serialization/DataSerializer.h"
#include "Serialization/Serializer.h"

bool PhysicalObject::isActive() {
	return gameObject->isActive();
}

PhysicalObject::PhysicalObject(GraphNode* _gameObject)
:Component(_gameObject, "Physical object") {}

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

SerializableType PhysicalObject::getSerializableType() {
	return SPhysicalObject;
}

Json::Value PhysicalObject::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["direction"] = DataSerializer::serializeVec3(direction);
	root["distance"] = distance;
	int counter = 0;
	for(auto i=lastKnownPosition.begin();i!=lastKnownPosition.end();++i) {
		root["lastKnownPosition"][counter++] = DataSerializer::serializeVec3(*i);
	}
	counter = 0;
	for(auto i=translations.begin();i!=translations.end();++i) {
		root["translations"][counter++] = DataSerializer::serializeVec3(*i);
	}
	root["isFalling"] = isFalling;
	root["mesh"] = serializer->serialize(mesh);
	root["gravity"] = gravity;
	return root;
}

void PhysicalObject::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	direction = DataSerializer::deserializeVec3(root["direction"]);
	distance = root["distance"].asFloat();
	for(int i=0;i<root["lastKnownPosition"].size();i++) {
		lastKnownPosition.push(DataSerializer::deserializeVec3(root["lastKnownPosition"][i]));
	}
	for(int i=0;i<root["translations"].size();i++) {
		translations.push(DataSerializer::deserializeVec3(root["translations"][i]));
	}
	isFalling = root["isFalling"].asBool();
	mesh = dynamic_cast<Mesh*>(serializer->deserialize(root["mesh"]).object);
	gravity = root["gravity"].asBool();
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
bool PhysicalObject::isOpaque() {return mesh != nullptr && mesh->isOpaque();}
