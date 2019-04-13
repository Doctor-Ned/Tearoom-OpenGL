#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H
#include "Collider.h"

class SphereCollider : public Collider
{
protected:
	float radius;
public:
	SphereCollider(GraphNode* _gameObject, glm::vec3 position, float radius);
	void setRadius(float size);
	float getRadius();
};
#endif