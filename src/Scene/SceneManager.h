#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "MainMenuScene.h"
#include "OptionsScene.h"
#include "GameScene.h"
#include "UboViewProjection.h"
#include "UboTextureColor.h"
#include "UboLights.h"
#include "GeometryShader.h"

class SceneManager {
public:
	static SceneManager& getInstance();
	void render();
	void update(double timeDelta);
	void newGame();
	void backToMenu();
	TextRenderer* getTextRenderer();
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	bool quitPending = false;
	Shader* getUiColorShader();
	Shader* getUiTextureShader();
	Shader* getSkyboxShader();
	Shader* getModelShader();
	Shader* getTextureShader();
	Shader* getColorShader();
	Shader* getReflectShader();
	Shader* getRefractShader();
	Shader* getDepthShader();
	GeometryShader* getDepthPointShader();
	Shader* getDepthDebugShader();
	UboLights* getUboLights();
	UboTextureColor* getUboTextureColor();
	UboViewProjection* getUboViewProjection();
	GLuint getFramebuffer();
	void setFramebuffer(GLuint fbo);
	bool inverseEnabled = false;
	float difficulty = 1.0f;
	SceneManager(SceneManager const&) = delete;
	void operator=(SceneManager const&) = delete;
protected:
	GLuint framebuffer;
	SceneManager() {}
	void setup();
	Shader *uiColorShader, *uiTextureShader, *skyboxShader, *modelShader, *textureShader, *colorShader, *reflectShader, *refractShader, *depthShader, *depthDebugShader;
	GeometryShader *depthPointShader;
	TextRenderer* textRenderer;
	Scene* currentScene = nullptr;
	MainMenuScene* mainMenuScene;
	OptionsScene* optionsScene;
	GameScene* gameScene;
	UboLights* uboLights;
	UboTextureColor* uboTextureColor;
	UboViewProjection* uboViewProjection;
};

#endif
