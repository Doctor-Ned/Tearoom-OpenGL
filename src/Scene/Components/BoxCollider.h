#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H
#include "Collider.h"

class BoxCollider : public Collider
{
private:
	glm::vec3 halfDimensions;
public:
	BoxCollider(GraphNode* _gameObject, glm::vec3 position, glm::vec3 half_dimensions);
	void setHalfDimensions(glm::vec3 half_dimensions);
	glm::vec3 getHalfDimensions();
};
#endif