#ifndef CAMERA_H
#define CAMERA_H

#include "Global.h"
#include "Scene/Components/Collider.h"
#include "Scene/Frustum.h"


class GraphNode;
class Camera : public Serializable {
public:
	Camera(glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
	       glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f)
	       , float speed = 3.0f, float rotSpeed = 6.0f);
	glm::mat4 getView();
	glm::mat4 getUntranslatedView();
	glm::vec3 getRight();
	glm::vec3 getFront();
	glm::vec3 getPos();
	glm::vec3 getUp();
	glm::vec3 getActualUp();
	Frustum getFrustum();
	float getYaw();
	float getPitch();
	void setPos(glm::vec3 pos);
	void rotateX(float timeDelta);
	void rotateY(float timeDelta);
	void moveForward(float timeDelta, int steps = 1);
	void moveBackward(float timeDelta, int steps = 1);
	void moveRight(float timeDelta, int steps = 1);
	void moveLeft(float timeDelta, int steps = 1);
	void moveUp(float timeDelta, int steps = 1);
	void moveDown(float timeDelta, int steps = 1);
	void setSpeed(float speed);
	void setRotSpeed(float rotSpeed);
	GraphNode* castRayFromCamera(glm::vec3 direction, float distance);
	void RecalculateFrustum();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	void recalculateFront();
	bool dirty = false;
	float speed;
	float rotSpeed;
	float yaw, pitch;
	glm::mat4 view;
	glm::vec3 cameraPos, cameraFront, cameraUp;
	Frustum frustum;
};

#endif
