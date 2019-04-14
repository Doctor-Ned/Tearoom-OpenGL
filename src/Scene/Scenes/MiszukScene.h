#ifndef MISZUK_SCENE
#define MISZUK_SCENE

#include "Scene.h"
#include "GLFW/glfw3.h"
#include "Ubo/UboLights.h"
#include "Ubo/UboTextureColor.h"
#include "Ubo/UboViewProjection.h"
#include <map>
#include "Render/Camera.h"

class MiszukScene : public Scene {
public:
	void render() override;
	void renderUi() override;
	void update(double deltaTime) override;
	MiszukScene();
	~MiszukScene();
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
protected:
	//--------------ANIMACJA---------------------
	bool f_keyPressed = false;
	//-----------------------------------------
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

#endif