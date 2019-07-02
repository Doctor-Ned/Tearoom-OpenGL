#include "CollisionSystem.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Scene/GraphNode.h"
#include "Scene/OctreeNode.h"
#include <iostream>
#include "nmmintrin.h"

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

	if (!collider1->isComponentActive() || !collider2->isComponentActive())
	{
		return collision;
	}
	if(!collider1->getIsTrigger() || !collider2->getIsTrigger())
	{
		
	}
	else if (collider1->getCollisionType() == STATIC && collider2->getCollisionType() == STATIC)
	{
		return collision;
	}

	if(collider2->getGameObject() == collider1->getGameObject() )
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

bool CollisionSystem::containTest(glm::vec3& min, glm::vec3& max, Collider* collider)
{
	if(collider->getType() == BoxCol)
	{
		float timebefore = glfwGetTime();
		BoxCollider* b_collider = static_cast<BoxCollider*>(collider);
		
		glm::vec3 halfDimension = b_collider->getHalfDimensions();
		glm::vec3 pos = b_collider->getPosition();
		/*SSEfloat4 minPoint, maxPoint, position, halfs;
		position.v = glm::vec4(b_collider->getPosition(), 1.0f);
		halfs.v = glm::vec4(b_collider->getHalfDimensions(), 1.0f);
		minPoint.f4 = _mm_sub_ps(position.f4, halfs.f4);
		maxPoint.f4 = _mm_add_ps(position.f4, halfs.f4);*/

		for(int i = 0; i < 3; i++)
		{
			if (pos[i] - halfDimension[i] <= min[i]
				|| pos[i] + halfDimension[i] >= max[i])
				return false;
		}
		float Time = glfwGetTime() - timebefore;
		return true;
	}
	if (collider->getType() == SphereCol) //testing it like a box inside other box (furthest distance from center in axis align directions)
	{
		SphereCollider* sphere = static_cast<SphereCollider*>(collider);
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
		BoxCollider* b_collider = static_cast<BoxCollider*>(collider);
		
		SSEfloat4 minPoint, maxPoint, position, halfs;
		position.v = glm::vec4(b_collider->getPosition(), 1.0f);
		halfs.v = glm::vec4(b_collider->getHalfDimensions(), 1.0f);
		minPoint.f4 = _mm_sub_ps(position.f4, halfs.f4);
		maxPoint.f4 = _mm_add_ps(position.f4, halfs.f4);
		
		for (int i = 0; i < 3; i++)
		{
			if(point[i] < minPoint.v[i]) return false;
			if(point[i] > maxPoint.v[i]) return false;
		}
		return true;
	}
	if (collider->getType() == SphereCol) //testing it like a box inside other box (furthest distance from center in axis aligned directions)
	{
		SphereCollider* sphere = static_cast<SphereCollider*>(collider);
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
	float k = 0.1f;
	static SphereCollider* sphere = new SphereCollider(nullptr, STATIC, true, startPoint, 0.01f);
	while (glm::distance(startPos, currentPos) < distance)
	{
		currentPos = startPos + direction * k;
		k += sphere->getRadius() * 2.0f;
		sphere->setPosition(currentPos);
		GraphNode* detectedNode = OctreeNode::findObjectByRayPoint(sphere, OctreeNode::getInstance(), toIgnore);
		if (detectedNode != nullptr) {
			//delete sphere;
			return detectedNode;
		}
	}
	//delete sphere;
	return nullptr;
}

bool CollisionSystem::SphereToSphere(Collider* collider1, Collider* collider2)
{
	SphereCollider* sphere1 = static_cast<SphereCollider*>(collider1);
	SphereCollider* sphere2 = static_cast<SphereCollider*>(collider2);
	glm::vec3 sphere2pos = sphere2->getPosition();
	glm::vec3 sphere1pos = sphere1->getPosition();

	SSEfloat4 pos2, pos1, result;
	pos2.v = glm::vec4(sphere2pos, 1.0f);
	pos1.v = glm::vec4(sphere1pos, 1.0f);
	result.f4 = _mm_sub_ps(pos2.f4, pos1.f4);
	float distanceSquared =
		pow(result.v.x, 2) +
		pow(result.v.y, 2) +
		pow(result.v.z, 2);
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
	BoxCollider* box1 = static_cast<BoxCollider*>(collider1);
	BoxCollider* box2 = static_cast<BoxCollider*>(collider2);
	glm::vec3 box1pos = box1->getPosition();
	glm::vec3 box2pos = box2->getPosition();
	glm::vec3 box1HDim = box1->getHalfDimensions();
	glm::vec3 box2HDim = box2->getHalfDimensions();
	glm::vec3 intersectionDepth(0);
	bool collision = false;
	for(int i = 0; i < 3; i++)
	{
		float distance = glm::distance(box1pos[i], box2pos[i]);
		float gapBetweenBoxes = distance - box1HDim[i] - box2HDim[i];
		if (gapBetweenBoxes > 0)
			return collision;

		intersectionDepth[i] = gapBetweenBoxes;
	}
	if(collision)
	{
		resolveAABBtoAABBCollision(box1, box2, intersectionDepth);
	}
	return true;
}

bool CollisionSystem::AABBtoSphere(Collider* _box, Collider* _sphere)
{
	//collider is sphere
	//finding closest point withing AABB to the sphere
	
	BoxCollider* box = static_cast<BoxCollider*>(_box);
	SphereCollider* sphere = static_cast<SphereCollider*>(_sphere);
	glm::vec3 closestPoint;

	SSEfloat4 position, halfs, minPoint, maxPoint;
	position.v = glm::vec4(box->getPosition(), 1.0f);
	halfs.v = glm::vec4(box->getHalfDimensions(), 1.0f);
	minPoint.f4 = _mm_sub_ps(position.f4, halfs.f4);
	maxPoint.f4 = _mm_add_ps(position.f4, halfs.f4);

	for (int i = 0; i < 3; i++)
	{
		float v = sphere->getPosition()[i]; // v = x, y, z (sphere center)
		if (v < minPoint.v[i])
		{
			v = minPoint.v[i];
		}
		if (v > maxPoint.v[i])
		{
			v = maxPoint.v[i];
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
