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
	glm::mat4 matrix;
	std::shared_ptr<Mesh> mesh_ptr = nullptr;
	glm::vec3 positionOffset;
public:
	bool isActive() override;
	Collider(ColliderType _type, GraphNode* _gameObject, glm::vec3 positionOffset);
	bool isOpaque() override;
	virtual ~Collider();
	glm::vec3 getPosition();
	void update(float mscec) override;
	void updateDrawData() override;
	void drawSelf(Shader* shader) override;
	ShaderType getShaderType() override;
	ColliderType getType();
	std::vector < std::function<int(Collider*)>> getCallbackFunctions();
	void setPositionOffset(glm::vec3 positionOffset);
	void setCollisionCallback(std::function<int(Collider*)> f);
};
#endif
