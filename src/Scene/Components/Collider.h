#ifndef COLLIDER_H
#define COLLIDER_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"

enum ColliderType
{
	SphereCollider,
	BoxCollider
};

class Collider : public Component, public Renderable
{
protected:
	std::vector < std::function<int(Collider*)>> callbackFunctions;
	ColliderType type;
	Mesh* colliderMesh;
	glm::mat4 mat, matrixWithoutRotation;
	glm::vec3 positionOffset;
	glm::vec4 data; // vec3 position, float radius or box edge size
public:
	void update(float mscec) override;
	void updateDrawData() override;
	void drawSelf(Shader* shader) override;
	ShaderType getShaderType() override;
	void SetCollisionCallback(std::function<int(Collider*)> f);
	Collider(ColliderType _type, GraphNode* _gameObject, glm::vec4 _data = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	virtual ~Collider();
	glm::vec4 getData();
	ColliderType getType();
	std::vector < std::function<int(Collider*)>> getCallbackFunctions();
};
#endif
