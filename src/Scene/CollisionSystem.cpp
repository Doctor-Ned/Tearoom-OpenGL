#include "CollisionSystem.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Scene/GraphNode.h"
#include "Scene/OctreeNode.h"
#include <iostream>

CollisionSystem::CollisionSystem()
{
}

CollisionSystem* CollisionSystem::getInstance()
{
	static CollisionSystem* collisionSystem;
	if(collisionSystem == nullptr)
	{
		collisionSystem = new CollisionSystem();
	}
	return collisionSystem;
}

CollisionSystem::~CollisionSystem()
{
}

bool CollisionSystem::checkCollision(Collider* collider1, Collider* collider2)
{
	bool collision = false;
	if (collider1->getType() == SphereCol && collider2->getType() == SphereCol)
	{
		collision = SphereToSphere(collider1, collider2);
	}
	else if (collider1->getType() == BoxCol && collider2->getType() == BoxCol)
	{
		collision = AABBtoAABB(collider1, collider2);
	}
	else if (collider1->getType() == BoxCol && collider2->getType() == SphereCol)
	{
		collision = AABBtoSphere(collider1, collider2);
	}
	else if (collider1->getType() == SphereCol && collider2->getType() == BoxCol)
	{
		collision = AABBtoSphere(collider2, collider1);
	}
	if (collision)
	{
		if (!collider1->getCallbackFunctions().empty())
		{
			for (auto & f : collider1->getCallbackFunctions())
			{
				f(collider2);
			}
		}
		if (!collider2->getCallbackFunctions().empty())
		{
			for (auto & f : collider2->getCallbackFunctions())
			{
				f(collider1);
			}
		}
	}
	return collision;
}

bool CollisionSystem::containTest(glm::vec3 min, glm::vec3 max, Collider* collider)
{
	if(collider->getType() == BoxCol)
	{	
		BoxCollider* b_collider = dynamic_cast<BoxCollider*>(collider);
		glm::vec3 halfDimension = b_collider->getHalfDimensions();
		for(int i = 0; i < 3; i++)
		{
			if (b_collider->getPosition()[i] - halfDimension[i] <= min[i] || b_collider->getPosition()[i] + halfDimension[i] >= max[i])
				return false;
		}
		return true;
	}
	if (collider->getType() == SphereCol) //testing it like a box inside other box (furthest distance from center in axis align directions)
	{
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider);
		float radius = sphere->getRadius();
		for(int i = 0; i < 3; i++)
		{
			if (sphere->getPosition()[i] - radius <= min[i] || sphere->getPosition()[i] + radius >= max[i])
				return false;
		}
		return true;
	}
	return false;
}

bool CollisionSystem::containTest(glm::vec3 point, Collider* collider)
{
	if (collider->getType() == BoxCol)
	{
		BoxCollider* b_collider = dynamic_cast<BoxCollider*>(collider);
		glm::vec3 halfDimension = b_collider->getHalfDimensions();
		for (int i = 0; i < 3; i++)
		{
			if(point[i] < b_collider->getPosition()[i] - halfDimension[i]) return false;
			if(point[i] > b_collider->getPosition()[i] + halfDimension[i]) return false;
		}
		return true;
	}
	if (collider->getType() == SphereCol) //testing it like a box inside other box (furthest distance from center in axis aligned directions)
	{
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider);
		float radius = sphere->getRadius();
		glm::vec3 sphereCenter = sphere->getPosition();
		float squareDistSpherePoint = pow(sphereCenter.x - point.x, 2) +
			pow(sphereCenter.y - point.y, 2) +
			pow(sphereCenter.z - point.z, 2);
		return squareDistSpherePoint < radius * radius;
	}
	return false;
}

GraphNode * CollisionSystem::castRay(glm::vec3 startPoint, glm::vec3 _direction, float distance)
{
	glm::vec3 startPos = startPoint;
	glm::vec3 direction = glm::normalize(_direction);
	glm::vec3 currentPos = startPos;
	float k = 0.5f;
	std::shared_ptr<OctreeNode> octree = OctreeNode::getInstance();
	while (glm::distance(startPos, currentPos) < distance)
	{
		currentPos = startPos + direction * k;
		k += 0.01f;

		GraphNode* detectedNode = OctreeNode::findObjectByRayPoint(currentPos, octree);
		if (detectedNode != nullptr) {
			return detectedNode;
		}
	}
	return nullptr;
}

bool CollisionSystem::SphereToSphere(Collider* collider1, Collider* coliider2)
{
	SphereCollider* sphere1 = dynamic_cast<SphereCollider*>(collider1);
	SphereCollider* sphere2 = dynamic_cast<SphereCollider*>(coliider2);
	float distanceSquared =
		pow(sphere2->getPosition().x - sphere1->getPosition().x, 2) +
		pow(sphere2->getPosition().y - sphere1->getPosition().y, 2) +
		pow(sphere2->getPosition().z - sphere1->getPosition().z, 2);
	float radiusSumSquared = pow(sphere1->getRadius() + sphere2->getRadius(), 2);
	return distanceSquared <= radiusSumSquared;
}

bool CollisionSystem::AABBtoAABB(Collider* collider1, Collider* collider2)
{
	BoxCollider* box1 = dynamic_cast<BoxCollider*>(collider1);
	BoxCollider* box2 = dynamic_cast<BoxCollider*>(collider2);
	//box with center point and half-width
	if (abs(box1->getPosition().x - box2->getPosition().x) > (box1->getHalfDimensions().x + box2->getHalfDimensions().x)) return false;
	if (abs(box1->getPosition().y - box2->getPosition().y) > (box1->getHalfDimensions().y + box2->getHalfDimensions().y)) return false;
	if (abs(box1->getPosition().z - box2->getPosition().z) > (box1->getHalfDimensions().z + box2->getHalfDimensions().z)) return false;
	return true;
}

bool CollisionSystem::AABBtoSphere(Collider* _box, Collider* _sphere)
{
	//collider is sphere
	//finding closest point withing AABB to the sphere
	
	BoxCollider* box = dynamic_cast<BoxCollider*>(_box);
	SphereCollider* sphere = dynamic_cast<SphereCollider*>(_sphere);
	glm::vec3 closestPoint;
	for (int i = 0; i < 3; i++)
	{
		float v = sphere->getPosition()[i]; // v = x, y, z (sphere center)
		if (v < box->getPosition()[i] - box->getHalfDimensions()[i])
		{
			v = box->getPosition()[i] - box->getHalfDimensions()[i];
		}
		if (v > box->getPosition()[i] + box->getHalfDimensions()[i])
		{
			v = box->getPosition()[i] + box->getHalfDimensions()[i];
		}
		closestPoint[i] = v;
	}

	//calculating squred distance from the nearest point to sphere center
	float sqDist = pow(sphere->getPosition().x - closestPoint.x, 2) +
		pow(sphere->getPosition().y - closestPoint.y, 2) +
		pow(sphere->getPosition().z - closestPoint.z, 2);

	//float distance = glm::distance(glm::vec3(_sphere->getPosition()), closestPoint);
	float sqRadius = sphere->getRadius() * sphere->getRadius();
	return sqDist <= sqRadius;
	//return distance <= _sphere->getData().w;
}