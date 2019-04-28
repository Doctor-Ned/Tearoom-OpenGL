#ifndef SPHERE_COLLIDER_H
#define SPHERE_COLLIDER_H
#include "Collider.h"

class SphereCollider : public Collider
{
protected:
	friend class Serializer;
	SphereCollider(){}
	float radius;
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	SphereCollider(GraphNode* _gameObject, Collision classification = STATIC, bool isTrigger = false, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 0.5f);
	void setRadius(float size);
	float getRadius();
	void renderGui() override;
};
#endif