#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H
#include "Collider.h"

class BoxCollider : public Collider
{
private:
	glm::vec3 halfDimensions;
public:
	BoxCollider(GraphNode* _gameObject, Collision classification = STATIC, bool isTrigger = false, glm::vec3 positionOffset = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 halfDims = glm::vec3(0.5f, 0.5f, 0.5f));
	void setHalfDimensions(glm::vec3 half_dimensions);
	glm::vec3 getHalfDimensions();
};
#endif