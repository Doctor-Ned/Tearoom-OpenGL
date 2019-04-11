#ifndef COLLIDER_H
#define COLLIDER_H
#include "Component.h"
#include "Mesh/Mesh.h"
enum ColliderType
{
	SphereCollider,
	BoxCollider
};

class Collider : public Component
{
protected:
	std::vector < std::function<int(Collider*)>> callbackFunctions;
	ColliderType type;
	std::shared_ptr<Mesh> mesh_ptr;
	glm::mat4 mat;
	glm::vec3 positionOffset;
	glm::vec4 data; // vec3 position, float radius or box edge size
public:
	void update(float mscec) override;
	void draw() override;
	void SetCollisionCallback(std::function<int(Collider*)> f);
	Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	virtual ~Collider();
	glm::vec4 getData();
	ColliderType getType();
	std::vector < std::function<int(Collider*)>> getCallbackFunctions();
	void changeOffset(glm::vec3 offset);
	void setSize(float size);
};
#endif
