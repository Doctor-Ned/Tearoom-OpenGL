#pragma once
#include "Scene.h"
#include "Render/TextRenderer.h"

class MenuScene;

class OptionsScene : public Scene {
public:
	OptionsScene(MenuScene *menuScene);
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
private:
	TextRenderer *textRenderer;
	MenuScene *menuScene;
};
