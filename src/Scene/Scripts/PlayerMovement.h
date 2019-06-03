#ifndef PLAYER_MOVEMENT_H
#define PLAYER_MOVEMENT_H
#include "Scene/Components/Component.h"

class Camera;
class Scene;
class PlayerMovement : public Component
{
protected:
	friend class Serializer;
	PlayerMovement(){}
	Camera* camera;
	bool refreshMousePosition = true;
	bool fly = false;
	bool gravity = true;
	float speed = 2.0f;
	float fastSpeed = 3.5f;
	glm::vec2 lastMousePosition;
	void renderGui() override;
public:
	static float mouseSensitivity;
	void setFly(bool fly);
	bool getFly();
	void setGravity(bool gravity);
	bool getGravity();
	Camera *getCamera();
	void setCamera(Camera *camera);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	PlayerMovement(GraphNode* _gameObject, Camera *camera);
	PlayerMovement(GraphNode* _gameObject);
	void update(float msec) override;
};
#endif