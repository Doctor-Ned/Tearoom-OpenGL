#include "CollisionSystem.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Scene/GraphNode.h"
#include "Scene/OctreeNode.h"
#include <iostream>
#include "Components/PhysicalObject.h"
#include "Components/Picking.h"

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
	else
	{
		if (collider1->getCollisionType() == DYNAMIC)
		{
			//PhysicalObject* physObj = collider1->getGameObject()->getComponent<PhysicalObject>();
			//if (physObj != nullptr)
			//{
			//	//collider1->getGameObject()->localTransform.setMatrix(collider1->getGameObject()->localTransform.getLastMatrix());
			//	/*glm::vec3 lastPos = collider1->getGameObject()->localTransform.getLastMatrix()[3];
			//	glm::vec3 actualPos = collider1->getGameObject()->localTransform.getPosition();
			//	float distance = glm::distance(lastPos, actualPos);
			//	glm::vec3 direction;
			//	if (distance == 0)
			//	{
			//		direction = glm::vec3(0);
			//	}
			//	else
			//	{
			//		direction = glm::normalize(actualPos - lastPos);
			//	}*/
			//	//collider1->getGameObject()->localTransform.translate(-direction * distance);
			//	physObj->pushTranslation(-physObj->direction * physObj->distance);
			//	//vektor kontaktu, g³êbokoœæ
			//}
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
	else
	{
		if (collider2->getCollisionType() == DYNAMIC)
		{
			//PhysicalObject* physObj = collider2->getGameObject()->getComponent<PhysicalObject>();
			//if(physObj != nullptr)
			//{
			//	//collider2->getGameObject()->localTransform.setMatrix(collider2->getGameObject()->localTransform.getLastMatrix());
			//	/*glm::vec3 lastPos = collider2->getGameObject()->localTransform.getLastMatrix()[3];
			//	glm::vec3 actualPos = collider2->getGameObject()->localTransform.getPosition();
			//	float distance = glm::distance(lastPos, actualPos);
			//	glm::vec3 direction;
			//	if (distance == 0)
			//	{
			//		direction = glm::vec3(0);
			//	}
			//	else
			//	{
			//		direction = glm::normalize(actualPos - lastPos);
			//	}*/
			//	//collider2->getGameObject()->localTransform.translate(-direction * distance);
			//	physObj->pushTranslation(-physObj->direction * physObj->distance);
			//}
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
	/*if (picking)
	{
		std::cout << "player" << std::endl;
		sphere2->getGameObject()->localTransform.translate(-dir * distance);
	}
	picking = sphere1->getGameObject()->getComponent<Picking>();
	if (picking)
	{
		std::cout << "player" << std::endl;
		sphere1->getGameObject()->localTransform.translate(dir * distance);
	}*/

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
