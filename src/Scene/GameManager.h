#pragma once

#include "Global.h"
#include "GLFW/glfw3.h"
#include <map>
//#include "Scenes/MenuScene.h"

class MenuScene;
class Scene;
class Camera;
class EditorScene;

struct Framebuffer {
	GLuint fbo = 0;
	GLuint depth = 0;
	GLuint texture = 0;
};

struct SpecialFramebuffer {
	GLuint fbo = 0;
	GLuint rbo = 0;
	GLuint texture = 0;
};

struct MultitextureFramebuffer {
	GLuint fbo = 0;
	GLuint depth = 0;
	GLuint *textures;
	unsigned int textureAmount = 0;
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
	EditorScene *getEditorScene();
	GameFramebuffers getFramebuffers();
	void setWindow(GLFWwindow *window);
	void setCursorLocked(bool locked);
	bool getCursorLocked();
	void goToMenu(bool destroyPreviousScene = true);
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight, bool updateUiProjection = true);
	void setup();
	Camera *getCamera() const;
	bool isVsyncEnabled() const;
	void setVsync(bool enabled);
	void addKeyCallback(int key, bool pressed, const std::function<void()>& callback);
	void addMouseCallback(int key, bool pressed, const std::function<void()>& callback);
	~GameManager();
	bool getKeyState(int key);
	bool getMouseState(int key);
	glm::vec2 getMousePosition() const;
	Scene* getCurrentScene();
	static GLuint createDepthRenderbuffer(GLsizei width, GLsizei height);
	static Framebuffer createFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, bool clamp = true, GLenum clampMode = GL_CLAMP_TO_EDGE, glm::vec4 border = glm::vec4(0.0f,0.0f,0.0f,0.0f));
	static SpecialFramebuffer createSpecialFramebuffer(GLenum textureTarget, GLfloat filter, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, bool clamp, GLenum attachment, GLenum clampMethod = GL_CLAMP_TO_EDGE);
	static MultitextureFramebuffer createMultitextureFramebuffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, int textureCount);
protected:
	void setKeyState(int key, bool pressed);
	void keyEvent(int key, bool pressed) const;
	std::map<int, bool> keyStates;
	std::map<int, std::map<bool, std::vector<std::function<void()>>>> keyCallbacks;
	void setMouseState(int key, bool pressed);
	void mouseEvent(int key, bool pressed);
	bool cursorLocked = false;
	std::map<int, bool> mouseStates;
	std::map<int, std::map<bool, std::vector<std::function<void()>>>> mouseCallbacks;
	bool enableVsync = true;
	glm::vec2 mousePosition;
	GLFWwindow *window;
	float windowHeight, windowWidth, windowCenterX, windowCenterY, screenWidth, screenHeight;
	Framebuffer uiFramebuffer, pingPongFramebuffers[2];
	MultitextureFramebuffer mainFramebuffer;
	GLuint renderbuffer;
	GameManager() {}
	Scene* currentScene = nullptr;
	MenuScene* menuScene = nullptr;
};
