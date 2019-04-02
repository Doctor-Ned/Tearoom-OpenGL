#ifndef COLLIDER_H
#define COLLIDER_H
#include "Component.h"
#include <string>
#include "Mesh/Mesh.h"
enum ColliderType
{
	SphereCollider,
	BoxCollider
};

class Collider : public Component
{
protected:
	ColliderType type;
	Mesh* colliderMesh;
	glm::vec3 positionOffset;
	glm::vec4 data; // vec3 position, float radius or box edge size
public:
	void update() override;
	void draw() override;
	bool checkCollision(Collider* collider);
	Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	virtual ~Collider();
};
#endif
