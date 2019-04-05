#pragma once
#include "Scene.h"
#include "Render/Camera.h"
#include "GLFW/glfw3.h"
#include <map>
#include "Ubo/UboLights.h"
#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Scene/DirLightNode.h"
#include "Scene/PointLightNode.h"
#include "Scene/SpotLightNode.h"
#include "Render/GeometryShader.h"
#include "Render/Skybox.h"

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
KEY_QUIT = GLFW_KEY_ESCAPE;

class TestScene : public Scene {
public:
	TestScene();
	void render() override;
	void renderUi() override;
	void update(double deltaTime) override;
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
protected:
	bool lockMouse = false;
	int renderDepthMap = 0;
	float spotNear = 1.0f, spotFar = 50.0f, dirNear = 4.0f, dirFar = 70.0f, dirProjSize = 10.0f;
	Skybox *skybox;
	void renderDirLights();
	void renderSpotLights();
	void renderPointLights();
	std::vector<Shader*> updatableShaders;
	glm::mat4 projection;
	UboLights *uboLights;
	UboTextureColor *uboTextureColor;
	UboViewProjection *uboViewProjection;
	std::vector<DirLight*> dirLights;
	std::vector<SpotLight*> spotLights;
	std::vector<PointLight*> pointLights;
	std::vector<LightShadowData*> dirLightShadows;
	std::vector<LightShadowData*> spotLightShadows;
	std::vector<LightShadowData*> pointLightShadows;
	std::vector<DirLightNode*> dirLightNodes;
	std::vector<SpotLightNode*> spotLightNodes;
	std::vector<PointLightNode*> pointLightNodes;
	MeshColorSphere *pointLightSphere;
	glm::mat4 spotLightProjection;
	Shader *depthShader, *depthDebugShader;
	GeometryShader *depthPointShader;
	DirLightNode *dirLightNode;
	SpotLightNode *spotLightNode;
	PointLightNode *pointLightNode;
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

