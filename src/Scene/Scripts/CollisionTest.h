#ifndef COLLISION_TEST_H
#define COLLISION_TEST_H
#include "Scene/Components/Component.h"
class Collider;
class CollisionTest : public Component
{
protected:
	friend class Serializer;
	CollisionTest(){}
	void applyCallback();
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer *serializer) override;
	int direction = 1;
	void update(float msec) override;
	~CollisionTest() override;
	void OnCollision(Collider* collider);
	CollisionTest(GraphNode* _gameObject);
};

#endif