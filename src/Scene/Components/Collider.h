#ifndef COLLIDER_H
#define COLLIDER_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"
#include <queue>

enum ColliderType
{
	SphereCol,
	BoxCol
};

enum Collision
{
	STATIC,
	DYNAMIC
};

class Collider abstract: public Component, public Renderable
{
protected:
	friend class Serializer;
	Collider() {}
	std::vector < std::function<void(Collider*)>> callbackFunctions;
	ColliderType type;
	Collision collisionType;
	bool isTrigger = false;
	glm::mat4 matrix;
	std::shared_ptr<Mesh> mesh_ptr = nullptr;
	glm::vec3 positionOffset;
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	bool isActive() override;
	Collider(ColliderType _type, GraphNode* _gameObject, Collision classification = STATIC, bool isTrigger = false, glm::vec3 positionOffset = glm::vec3(0.0f, 0.0f, 0.0f));
	bool isOpaque() override;
	virtual ~Collider();
	glm::vec3 getPosition();
	void update(float mscec) override;
	void updateDrawData() override;
	void drawSelf(Shader* shader) override;
	ShaderType getShaderType() override;
	ColliderType getType();
	Collision getCollisionType();
	bool getIsTrigger();
	void setMatrix(glm::mat4 mat);
	void setPosition(glm::vec3& pos);
	void setIsTrigger(bool _isTrigger);
	std::vector < std::function<void(Collider*)>> getCallbackFunctions();
	void setPositionOffset(glm::vec3 positionOffset);
	void setCollisionCallback(std::function<void(Collider*)> f);
	void renderGui() override;
};
#endif
