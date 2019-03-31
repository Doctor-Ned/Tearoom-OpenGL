#ifndef CAMERA_H
#define CAMERA_H

#include "Global.h"

class Camera {
public:
	Camera(glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
	       glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f)
	       , float speed = 3.0f, float rotSpeed = 6.0f);
	glm::mat4 getView();
	glm::mat4 getUntranslatedView();
	glm::vec3 getFront();
	glm::vec3 getPos();
	glm::vec3 getUp();
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
protected:
	void recalculateFront();
	bool dirty = false;
	float speed;
	float rotSpeed;
	float yaw, pitch;
	glm::mat4 view;
	glm::vec3 cameraPos, cameraFront, cameraUp;
};

#endif
