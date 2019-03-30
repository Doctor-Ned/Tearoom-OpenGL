#pragma once
#include "Scene/Scene.h"
#include "Render/Camera.h"
#include "GLFW/glfw3.h"
#include <map>
#include "Ubo/UboLights.h"
#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"

const int
KEY_FORWARD = GLFW_KEY_W,
KEY_BACKWARD = GLFW_KEY_S,
KEY_LEFT = GLFW_KEY_A,
KEY_RIGHT = GLFW_KEY_D,
KEY_UP = GLFW_KEY_E,
KEY_DOWN = GLFW_KEY_Q,
KEY_FAST = GLFW_KEY_LEFT_SHIFT,
KEY_SLOW = GLFW_KEY_LEFT_CONTROL,
KEY_MOUSE_LEFT = GLFW_KEY_LEFT,
KEY_MOUSE_RIGHT = GLFW_KEY_RIGHT,
KEY_MOUSE_UP = GLFW_KEY_UP,
KEY_MOUSE_DOWN = GLFW_KEY_DOWN;

class TestScene : public Scene {
public:
	TestScene();
	void render() override;
	void update(double deltaTime) override;
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
protected:
	std::vector<Shader*> updatableShaders;
	glm::mat4 projection;
	UboLights *uboLights;
	UboTextureColor *uboTextureColor;
	UboViewProjection *uboViewProjection;
	bool getKeyState(int key);
	void setKeyState(int key, bool pressed);
	void keyEvent(int key, bool pressed);
	std::map<int, bool> keyStates;
	Camera *camera;
	const float BASE_MOVEMENT_SPEED = 1.0f;
	float movementSpeed = BASE_MOVEMENT_SPEED;
	float mouseX, mouseY;
	float mouseMovementX, mouseMovementY;
	bool initMouse = true;
};

