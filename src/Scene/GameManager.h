#pragma once

#include "Global.h"
#include "GLFW/glfw3.h"
//#include "Scenes/MenuScene.h"

class MenuScene;
class Scene;

struct Framebuffer {
	GLuint fbo;
	GLuint texture;
};

struct MultitextureFramebuffer {
	GLuint fbo;
	GLuint *textures;
	unsigned int textureAmount;
};

struct GameFramebuffers {
	MultitextureFramebuffer main;
	Framebuffer ping;
	Framebuffer pong;
	Framebuffer ui;
};

class GameManager {
public:
	static GameManager *getInstance();
	void render();
	void renderUi();
	void update(double timeDelta);
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	GLFWwindow* getWindow();
	void quit();
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
	GLuint getMainFramebuffer();
	GameFramebuffers getFramebuffers();
	void setWindow(GLFWwindow *window);
	void setCursorLocked(bool locked);
	void goToMenu(bool destroyPreviousScene = true);
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	void setup();
	bool useLight = true;
	bool castShadows = true;
	~GameManager();
protected:
	GLuint createDepthRenderbuffer(GLsizei width, GLsizei height);
	Framebuffer createFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type);
	MultitextureFramebuffer createMultitextureFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, int textureCount);
	GLFWwindow *window;
	float windowHeight, windowWidth, windowCenterX, windowCenterY, screenWidth, screenHeight;
	Framebuffer uiFramebuffer, pingPongFramebuffers[2];
	MultitextureFramebuffer mainFramebuffer;
	GLuint renderbuffer;
	GameManager() {}
	Scene* currentScene = nullptr;
	MenuScene* menuScene;
};
