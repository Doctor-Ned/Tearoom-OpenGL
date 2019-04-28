#pragma once
#include "Scene.h"
#include "Render/TextRenderer.h"
#include "Render/PostProcessingShader.h"

class MenuScene;

class OptionsScene : public Scene {
public:
	OptionsScene(MenuScene *menuScene);
	~OptionsScene();
	void update(double deltaTime) override;
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
private:
	void load();
	void save();
	TextRenderer *textRenderer;
	MenuScene *menuScene;
	const char* SETTINGS_FILE = "settings.json";
	PostProcessingShader* pps;
};
