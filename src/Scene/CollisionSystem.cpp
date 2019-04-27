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
	if(collider1->getCollisionType() == STATIC && collider2->getCollisionType() == STATIC)
	{
		return collision;
	}

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
	
	if (!collision)
	{
		return false;
	}

	if (collider1->getIsTrigger())
	{
		if (!collider1->getCallbackFunctions().empty())
		{
			for (auto & f : collider1->getCallbackFunctions())
			{
				f(collider2);
			}
		}
	}
	
	if (collider2->getIsTrigger())
	{
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

GraphNode * CollisionSystem::castRay(glm::vec3 startPoint, glm::vec3 _direction, float distance, Collider* toIgnore)
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

		GraphNode* detectedNode = OctreeNode::findObjectByRayPoint(currentPos, octree, toIgnore);
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
	
	bool collision = distanceSquared <= radiusSumSquared;
	
	if(collision)
	{
		resolveSphereToSphereCollision(sphere1, sphere2);
	}
	return collision;

}

bool CollisionSystem::AABBtoAABB(Collider* collider1, Collider* collider2)
{
	BoxCollider* box1 = dynamic_cast<BoxCollider*>(collider1);
	BoxCollider* box2 = dynamic_cast<BoxCollider*>(collider2);
	glm::vec3 intersectionDepth(0);
	for(int i = 0; i < 3; i++)
	{
		float distance = glm::distance(box1->getPosition()[i], box2->getPosition()[i]);
		float gapBetweenBoxes = distance - box1->getHalfDimensions()[i] - box2->getHalfDimensions()[i];
		if (gapBetweenBoxes > 0)
			return false;

		intersectionDepth[i] = gapBetweenBoxes;
	}
	resolveAABBtoAABBCollision(box1, box2, intersectionDepth);
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
	bool collision = sqDist <= sqRadius;

	if(collision)
	{
		resolveAABBtoSphereCollision(box, sphere, closestPoint);
	}
	return collision;
}

void CollisionSystem::resolveSphereToSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2)
{
	if(sphere1->getIsTrigger() == true || sphere2->getIsTrigger() == true)
	{
		return;
	}

	glm::vec3 dir = glm::normalize(sphere1->getPosition() - sphere2->getPosition());
	glm::vec3 r1 = sphere1->getPosition() + (-dir) * sphere1->getRadius();
	glm::vec3 r2 = sphere2->getPosition() + dir * sphere2->getRadius();
	float distance = glm::distance(r1, r2);

	if (sphere1->getCollisionType() == DYNAMIC && sphere2->getCollisionType() == DYNAMIC)
	{
		sphere1->getGameObject()->localTransform.translate(dir * distance / 2.0f);
		sphere2->getGameObject()->localTransform.translate(-dir * distance / 2.0f);
	}
	else if (sphere1->getCollisionType() == DYNAMIC)
	{
		sphere1->getGameObject()->localTransform.translate(dir * distance);
	}
	else if (sphere2->getCollisionType() == DYNAMIC)
	{
		sphere2->getGameObject()->localTransform.translate(-dir * distance);
	}
}

void CollisionSystem::resolveAABBtoSphereCollision(BoxCollider* box, SphereCollider* sphere, glm::vec3& closestPoint)
{
	if (box->getIsTrigger() == true || sphere->getIsTrigger() == true)
	{
		return;
	}
	glm::vec3 dir = glm::normalize(closestPoint - sphere->getPosition());
	glm::vec3 r1 = sphere->getPosition() + dir * sphere->getRadius();
	float distance = glm::distance(closestPoint, r1);

	if(box->getCollisionType() == DYNAMIC && sphere->getCollisionType() == DYNAMIC)
	{
		sphere->getGameObject()->localTransform.translate(-dir * distance / 2.0f);
		box->getGameObject()->localTransform.translate(dir * distance / 2.0f);
	}
	else if(box->getCollisionType() == DYNAMIC)
	{
		box->getGameObject()->localTransform.translate(dir * distance);
	}
	else if (sphere->getCollisionType() == DYNAMIC)
	{
		sphere->getGameObject()->localTransform.translate(-dir * distance);
	}
}

void CollisionSystem::resolveAABBtoAABBCollision(BoxCollider* box1, BoxCollider* box2, glm::vec3& depthBox)
{
	if (box1->getIsTrigger() == true || box2->getIsTrigger() == true)
	{
		return;
	}

	int axis = 0;
	float smallestDepth = depthBox.x;
	for(int i = 1; i < 3; i++)
	{
		if(depthBox[i] > smallestDepth)
		{
			smallestDepth = depthBox[i];
			axis = i;
		}
	}
	glm::vec3 translation(0);

	translation[axis] = smallestDepth;

	float distance = glm::distance(box1->getPosition(), box2->getPosition());
	float distAfterTranslation = glm::distance(box1->getPosition() + translation, box2->getPosition());
	float direction = 0.0f;
	if(distance > distAfterTranslation)
	{
		direction = -1.0f;
	}
	else
	{
		direction = 1.0f;
	}

	if(box1->getCollisionType() == DYNAMIC && box2->getCollisionType() == DYNAMIC)
	{
		box1->getGameObject()->localTransform.translate(direction * translation / 2.0f);
		box2->getGameObject()->localTransform.translate(direction * -translation / 2.0f);
	}
	else if(box1->getCollisionType() == DYNAMIC)
	{
		box1->getGameObject()->localTransform.translate(direction * translation);
	}
	else if (box2->getCollisionType() == DYNAMIC)
	{
		box2->getGameObject()->localTransform.translate(direction * -translation);
	}
	
}
