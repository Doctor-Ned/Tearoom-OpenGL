#include "CollisionTest.h"
#include "Scene/GraphNode.h"
#include "Scene/Components/Collider.h"

CollisionTest::CollisionTest(GraphNode* _gameObject) : Component(_gameObject)
{
	applyCallback();
}

void CollisionTest::applyCallback() {
	auto f = [this](Collider* collider) {
		this->OnCollision(collider);
	};
	gameObject->getComponent<Collider>()->setCollisionCallback(f);
}

SerializableType CollisionTest::getSerializableType() {
	return SCollisionTest;
}

Json::Value CollisionTest::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["direction"] = direction;
	return root;
}

void CollisionTest::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	direction = root["direction"].asInt();
	applyCallback();
}

void CollisionTest::update(float msec)
{
	gameObject->localTransform.rotateYDegrees(40.0f*msec*direction);
}

CollisionTest::~CollisionTest()
{
}

void CollisionTest::OnCollision(Collider* collider)
{
	direction *= -1;
	gameObject->localTransform.rotateYDegrees(2.0f*direction);
	/*Collider* collider2 = gameObject->getComponent<Collider>();
	if (direction > 0)
		collider2->setSize(1.5f);
	else
	{
		collider2->setSize(1.75f);
	}*/
}


