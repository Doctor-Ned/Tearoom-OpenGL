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
	inline bool SphereToSphere(Collider* _sphere1, Collider* _sphere2);
	inline bool AABBtoAABB(Collider* _box1, Collider* _box2);
	inline bool AABBtoSphere(Collider* _box, Collider* _sphere);
	ColliderType type;
	Mesh* colliderMesh;
	glm::mat4 mat;
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
