#include "CollisionTest.h"
#include "Scene/GraphNode.h"
#include "Scene/Components/Collider.h"

CollisionTest::CollisionTest(GraphNode* _gameObject) : Component(_gameObject)
{
	auto f = [this](Collider* collider)
	{
		this->OnCollision(collider);
		return 1;
	};
	gameObject->getComponent<Collider>()->setCollisionCallback(f);
}

void CollisionTest::update(float msec)
{
	gameObject->localTransform.rotate(40.0f * msec * direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

CollisionTest::~CollisionTest()
{
}

void CollisionTest::OnCollision(Collider* collider)
{
	direction *= -1;
	gameObject->localTransform.rotate(2.0f * direction, glm::vec3(0.0f, 1.0f, 0.0f));
	/*Collider* collider2 = gameObject->getComponent<Collider>();
	if (direction > 0)
		collider2->setSize(1.5f);
	else
	{
		collider2->setSize(1.75f);
	}*/
}


