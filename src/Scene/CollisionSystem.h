#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include <glm/vec3.hpp>

class Collider;
class GraphNode;
class CollisionSystem
{
private:
	CollisionSystem();
	inline bool SphereToSphere(Collider* _sphere1, Collider* _sphere2);
	inline bool AABBtoAABB(Collider* _box1, Collider* _box2);
	inline bool AABBtoSphere(Collider* _box, Collider* _sphere);
public:
	static CollisionSystem* getInstance();
	~CollisionSystem();
	bool checkCollision(Collider* collider1, Collider* collider2);
	bool containTest(glm::vec3 min, glm::vec3 max, Collider* collider);
	bool containTest(glm::vec3 point, Collider* collider);
	GraphNode* castRay(glm::vec3 startPoint, glm::vec3 _direction, float distance);
};
#endif