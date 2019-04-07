#ifndef COLLISION_TEST_H
#define COLLISION_TEST_H
#include "Scene/Components/Component.h"
class Collider;
class CollisionTest : public Component
{
public:
	int direction = 1;
	void update(float msec) override;
	void draw() override;
	~CollisionTest() override;
	void OnCollision(Collider* collider);
	CollisionTest(GraphNode* _gameObject);
};

#endif