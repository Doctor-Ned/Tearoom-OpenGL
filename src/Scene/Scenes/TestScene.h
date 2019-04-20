#pragma once
#include "Scene.h"
#include "Render/Camera.h"
#include "GLFW/glfw3.h"
#include "Ubo/UboLights.h"
#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Render/GeometryShader.h"
#include "Render/Skybox.h"
#include "Render/LightManager.h"
#include "Scene/Components/LightComponents/Sun.h"

class DirLightComp;
class SpotLightComp;
class PointLightComp;
class MeshColorSphere;

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
KEY_MOUSE_DOWN = GLFW_KEY_DOWN,
KEY_TOGGLE_MOUSE_LOCK = GLFW_KEY_SPACE,
KEY_QUIT = GLFW_KEY_ESCAPE,
KEY_INTERACT = GLFW_KEY_F;

class TestScene : public Scene {
public:
	TestScene();
	void render() override;
	void renderUi() override;
	Camera *getCamera() override;
	void update(double deltaTime) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
protected:
	void keyEvent(int key, bool pressed) override;
	bool lockMouse = false;
	int renderDepthMap = 0;
	float spotNear = 1.0f, spotFar = 50.0f, dirNear = 4.0f, dirFar = 70.0f, dirProjSize = 10.0f;
	Skybox *skybox;
	std::vector<Shader*> updatableShaders;
	glm::mat4 projection;
	Lights lights;
	UboLights *uboLights;
	UboTextureColor *uboTextureColor;
	UboViewProjection *uboViewProjection;
	std::vector<DirLightComp*> dirLightComps;
	std::vector<SpotLightComp*> spotLightComps;
	std::vector<PointLightComp*> pointLightComps;
	GraphNode *modelNode = nullptr;
	MeshColorSphere *pointLightSphere;
	GraphNode *sunNode;
	Sun *sun;
	Camera *camera;
	const float BASE_MOVEMENT_SPEED = 1.0f;
	float movementSpeed = BASE_MOVEMENT_SPEED;
	float mouseX, mouseY;
	float mouseMovementX, mouseMovementY;
	bool initMouse = true;
};

