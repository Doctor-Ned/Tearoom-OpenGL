#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include <glm/vec3.hpp>

class Collider;
class GraphNode;
class SphereCollider;
class BoxCollider;
class CollisionSystem
{
private:
	CollisionSystem();
	inline bool SphereToSphere(Collider* _sphere1, Collider* _sphere2);
	inline bool AABBtoAABB(Collider* _box1, Collider* _box2);
	inline bool AABBtoSphere(Collider* _box, Collider* _sphere);
	inline void resolveSphereToSphereCollision(SphereCollider* _sphere1, SphereCollider* _sphere2);
	inline void resolveAABBtoSphereCollision(BoxCollider* _box, SphereCollider* _sphere, glm::vec3& closestPoint);
	inline void resolveAABBtoAABBCollision(BoxCollider* _box, BoxCollider* _box2, glm::vec3& depthBox);
public:
	static CollisionSystem* getInstance();
	~CollisionSystem();
	bool checkCollision(Collider* collider1, Collider* collider2);
	bool containTest(glm::vec3& min, glm::vec3& max, Collider* collider);
	bool containTest(glm::vec3 point, Collider* collider);
	GraphNode* castRay(glm::vec3 startPoint, glm::vec3 _direction, float distance, Collider* toIgnore = nullptr);
};
#endif