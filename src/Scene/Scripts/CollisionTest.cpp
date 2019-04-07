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
	gameObject->getComponent<Collider>()->SetCollisionCallback(f);
}

void CollisionTest::update(float msec)
{
	gameObject->localTransform.Rotate(40.0f * msec * direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void CollisionTest::draw()
{
}

CollisionTest::~CollisionTest()
{
}

void CollisionTest::OnCollision(Collider* collider)
{
	direction *= -1;
	gameObject->localTransform.Rotate(2.0f * direction, glm::vec3(0.0f, 1.0f, 0.0f));
}


