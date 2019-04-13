#ifndef COLLIDER_H
#define COLLIDER_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"

enum ColliderType
{
	SphereCol,
	BoxCol
};

class Collider abstract: public Component, public Renderable
{
protected:
	std::vector < std::function<int(Collider*)>> callbackFunctions;
	ColliderType type;
	glm::mat4 mat, matrixWithoutRotation;
	std::shared_ptr<Mesh> mesh_ptr;
	glm::vec3 positionOffset;
	glm::vec3 position;
public:
	Collider(ColliderType _type, GraphNode* _gameObject, glm::vec3 position);
	virtual ~Collider();
	void update(float mscec) override;
	void updateDrawData() override;
	void drawSelf(Shader* shader) override;
	glm::vec3 getPosition();
	ShaderType getShaderType() override;
	ColliderType getType();
	std::vector < std::function<int(Collider*)>> getCallbackFunctions();
	void setPosition(glm::vec3 pos);
	void setCollisionCallback(std::function<int(Collider*)> f);
};
#endif
