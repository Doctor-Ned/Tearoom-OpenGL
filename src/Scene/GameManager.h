#pragma once

#include "Global.h"
#include "GLFW/glfw3.h"
#include "Scenes/MenuScene.h"

class GameManager {
public:
	static GameManager *getInstance();
	void render();
	void update(double timeDelta);
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	GLFWwindow* getWindow();
	void quit();
	GLuint getFramebuffer();
	void setFramebuffer(GLuint fbo);
	bool inverseEnabled = false;
	float difficulty = 1.0f;
	GameManager(GameManager const&) = delete;
	void operator=(GameManager const&) = delete;
	void setCurrentScene(Scene* scene);
	void setCurrentSceneAndDeletePrevious(Scene* scene);
	float getWindowWidth();
	float getWindowHeight();
	float getScreenWidth();
	float getScreenHeight();
	float getWindowCenterX();
	float getWindowCenterY();
	void setWindow(GLFWwindow *window);
	void setCursorLocked(bool locked);
	void goToMenu(bool destroyPreviousScene = true);
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	void setup();
	bool useLight = true;
	bool castShadows = true;
	int spotDirShadowTexelResolution = 3; // a non-negative power of 3. 
	int pointShadowSamples = 20;          // 20 is nice but we can experiment with some other values.
	~GameManager();
protected:
	GLFWwindow *window;
	float windowHeight, windowWidth, windowCenterX, windowCenterY, screenWidth, screenHeight;
	GLuint framebuffer;
	GameManager() {}
	Scene* currentScene = nullptr;
	MenuScene* menuScene;
};
