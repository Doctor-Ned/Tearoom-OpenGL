#include "CollisionSystem.h"
#include "Components/Collider.h"

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
	if (collider1->getType() == SphereCollider && collider2->getType() == SphereCollider)
	{
		collision = SphereToSphere(collider1, collider2);
	}
	else if (collider1->getType() == BoxCollider && collider2->getType() == BoxCollider)
	{
		collision = AABBtoAABB(collider1, collider2);
	}
	else if (collider1->getType() == BoxCollider && collider2->getType() == SphereCollider)
	{
		collision = AABBtoSphere(collider1, collider2);
	}
	else if (collider1->getType() == SphereCollider && collider2->getType() == BoxCollider)
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
	if(collider->getType() == BoxCollider)
	{	
		float halfDimension = collider->getData().w;
		for(int i = 0; i < 3; i++)
		{
			if (collider->getData()[i] - halfDimension <= min[i] || collider->getData()[i] + halfDimension >= max[i])
				return false;
		}
		return true;
	}
	if (collider->getType() == SphereCollider) //testing it like a box inside other box (furthest distance from center in axis align directions)
	{
		float radius = collider->getData().w;
		for(int i = 0; i < 3; i++)
		{
			if (collider->getData()[i] - radius <= min[i] || collider->getData()[i] + radius >= max[i])
				return false;
		}
		return true;
	}
}

bool CollisionSystem::containTest(glm::vec3 point, Collider* collider)
{
	if (collider->getType() == BoxCollider)
	{
		float halfDimension = collider->getData().w;
		for (int i = 0; i < 3; i++)
		{
			if (point[i] < collider->getData()[i] - halfDimension || point[i] > collider->getData()[i] + halfDimension)
				return false;
		}
		return true;
	}
	if (collider->getType() == SphereCollider) //testing it like a box inside other box (furthest distance from center in axis align directions)
	{
		float radius = collider->getData().w;
		glm::vec3 sphereCenter = collider->getData();
		float distSpherePoint = glm::distance(sphereCenter, point);
		return distSpherePoint < radius;
	}
}

bool CollisionSystem::SphereToSphere(Collider* _sphere1, Collider* _sphere2)
{
	float distanceSquared =
		pow(_sphere2->getData().x - _sphere1->getData().x, 2) +
		pow(_sphere2->getData().y - _sphere1->getData().y, 2) +
		pow(_sphere2->getData().z - _sphere1->getData().z, 2);
	float radiusSumSquared = pow(_sphere1->getData().w + _sphere2->getData().w, 2);
	return distanceSquared <= radiusSumSquared;
}

bool CollisionSystem::AABBtoAABB(Collider* _box1, Collider* _box2)
{
	//box with center point and half-width
	if (abs(_box1->getData().x - _box2->getData().x) > (_box1->getData().w + _box2->getData().w)) return false;
	if (abs(_box1->getData().y - _box2->getData().y) > (_box1->getData().w + _box2->getData().w)) return false;
	if (abs(_box1->getData().z - _box2->getData().z) > (_box1->getData().w + _box2->getData().w)) return false;
	return true;
}

bool CollisionSystem::AABBtoSphere(Collider* _box, Collider* _sphere)
{
	//collider is sphere
	//finding closest point withing AABB to the sphere
	glm::vec3 closestPoint;
	for (int i = 0; i < 3; i++)
	{
		float v = _sphere->getData()[i]; // v = x, y, z (sphere center)
		if (v < _box->getData()[i] - _box->getData().w)
		{
			v = _box->getData()[i] - _box->getData().w;
		}
		if (v > _box->getData()[i] + _box->getData().w)
		{
			v = _box->getData()[i] + _box->getData().w;
		}
		closestPoint[i] = v;
	}

	//calculating squred distance from the nearest point to sphere center
	float sqDist = pow(_sphere->getData().x - closestPoint.x, 2) +
		pow(_sphere->getData().y - closestPoint.y, 2) +
		pow(_sphere->getData().z - closestPoint.z, 2);

	//float distance = glm::distance(glm::vec3(_sphere->getData()), closestPoint);
	float sqRadius = _sphere->getData().w * _sphere->getData().w;
	return sqDist <= sqRadius;
	//return distance <= _sphere->getData().w;
}