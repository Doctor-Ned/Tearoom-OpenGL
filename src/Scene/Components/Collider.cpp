#include "Collider.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/SceneManager.h"
#include <iostream>

void Collider::update()
{
	glm::vec3 gameObjectPos = gameObject->worldTransform.getPosition();
	
	glm::mat4 tmp(1);
	tmp[3][0] = positionOffset.x;
	tmp[3][1] = positionOffset.y;
	tmp[3][2] = positionOffset.z;
	mat = gameObject->worldTransform.Matrix() * tmp;
	data.x = mat[3][0];
	data.y = mat[3][1];
	data.z = mat[3][2];
	//std::cout << data.x << " " << data.y << " " << data.z << " " << data.w << std::endl;
	//std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;
}

void Collider::draw()
{
	//colliderMesh->draw(SceneManager::getInstance()->getShader(STColor), gameObject->worldTransform.Matrix(), 1.0f);
	colliderMesh->draw(mat);
}

bool Collider::checkCollision(Collider* collider)
{
	bool collision = false;
	if (type == SphereCollider && collider->type == SphereCollider)
	{
		collision = SphereToSphere(this, collider);
	}
	else if(type == BoxCollider && collider->type == BoxCollider)
	{
		collision = AABBtoAABB(this, collider);
	}
	else if (type == BoxCollider && collider->type == SphereCollider)
	{
		collision = AABBtoSphere(this, collider);
	}
	else if (type == SphereCollider && collider->type == BoxCollider)
	{
		collision = AABBtoSphere(collider, this);
	}
	return collision;
}

bool Collider::SphereToSphere(Collider* _sphere1, Collider* _sphere2)
{
	float distanceSquared =
		pow(_sphere2->data.x - _sphere1->data.x, 2) +
		pow(_sphere2->data.y - _sphere1->data.y, 2) +
		pow(_sphere2->data.z - _sphere1->data.z, 2);
	float radiusSumSquared = pow(_sphere1->data.w + _sphere2->data.w, 2);
	return distanceSquared <= radiusSumSquared;
}

bool Collider::AABBtoAABB(Collider* _box1, Collider* _box2)
{
	//box with center point and half-width
	if (abs(_box1->data.x - _box2->data.x) > (_box1->data.w + _box2->data.w)) return false;
	if (abs(_box1->data.y - _box2->data.y) > (_box1->data.w + _box2->data.w)) return false;
	if (abs(_box1->data.z - _box2->data.z) > (_box1->data.w + _box2->data.w)) return false;
	return true;
}

bool Collider::AABBtoSphere(Collider* _box, Collider* _sphere)
{
	//collider is sphere
	//finding closest point withing AABB to the sphere
	glm::vec3 closestPoint;
	for (int i = 0; i < 3; i++)
	{
		float v = _sphere->data[i]; // v = x, y, z (sphere center)
		if (v < _box->data[i] - _box->data.w) v = _box->data[i] - _box->data.w;
		if (v > _box->data[i] + _box->data.w) v = _box->data[i] + _box->data.w;
		closestPoint[i] = v;
	}
	//calculating squred distance from the nearest point to sphere center
	float sqDist = pow(_sphere->data.x + closestPoint.x, 2) +
		pow(_sphere->data.y + closestPoint.y, 2) +
		pow(_sphere->data.z + closestPoint.z, 2);

	float sqRadius = _sphere->data.w * _sphere->data.w;
	
	return sqDist <= sqRadius;
}

Collider::Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data) 
: Component(_gameObject), type(_type), positionOffset(_data), data(_data)
{
	if(type == SphereCollider)
	{
		colliderMesh = new MeshColorSphere(data.w, 15, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	if(type == BoxCollider)
	{
		colliderMesh = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	colliderMesh->setUseLight(false);
	colliderMesh->setRenderMode(GL_LINES);

	mat = glm::mat4(1);
}

Collider::~Collider()
{
	
}
