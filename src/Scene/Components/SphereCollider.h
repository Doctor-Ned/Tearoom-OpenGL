#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H
#include "Collider.h"

class SphereCollider : public Collider
{
protected:
	float radius;
public:
	SphereCollider(GraphNode* _gameObject, Collision classification = STATIC, bool isTrigger = false, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 0.5f);
	void setRadius(float size);
	float getRadius();
};
#endif