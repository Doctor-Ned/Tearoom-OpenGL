#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Ubo/UboLights.h"
#include "Render/GeometryShader.h"
#include "Render/TextRenderer.h"
#include "MenuScene.h"
#include "Scene.h"
#include "Global.h"
#include "GLFW/glfw3.h"

enum ShaderType {
	STNone,
	STSkybox,
	STTexture,
	STColor,
	STModel,
	STModelInstanced,
	STReflect,
	STRefract,
	STDepth,
	STDepthPoint,
	STDepthDebug,
	STUiTexture,
	STUiColor
};

class SceneManager {
public:
	static SceneManager *getInstance();
	void render();
	void update(double timeDelta);
	TextRenderer* getTextRenderer();
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	GLFWwindow* getWindow();
	void quit();
	UboLights* getUboLights();
	UboTextureColor* getUboTextureColor();
	UboViewProjection* getUboViewProjection();
	GLuint getFramebuffer();
	void setFramebuffer(GLuint fbo);
	bool inverseEnabled = false;
	float difficulty = 1.0f;
	SceneManager(SceneManager const&) = delete;
	void operator=(SceneManager const&) = delete;
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
	Shader* getShader(ShaderType type);
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	void setup();
	bool useLight = true;
	bool castShadows = true;
	int spotDirShadowTexelResolution = 3; // a non-negative power of 3. 
	int pointShadowSamples = 20;          // 20 is nice but we can experiment with some other values.
	~SceneManager();
protected:
	GLFWwindow *window;
	std::vector<Ubo*> ubos;
	std::map<ShaderType, Shader*> shaders;
	float windowHeight, windowWidth, windowCenterX, windowCenterY, screenWidth, screenHeight;
	GLuint framebuffer;
	SceneManager() {}
	GeometryShader *depthPointShader;
	TextRenderer* textRenderer;
	Scene* currentScene = nullptr;
	MenuScene* menuScene;
	UboLights* uboLights;
	UboTextureColor* uboTextureColor;
	UboViewProjection* uboViewProjection;
};

#endif
