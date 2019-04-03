#pragma once
#include "Scene.h"
#include "OptionsScene.h"


class MenuScene : public Scene {
public:
	MenuScene();
	void render() override;
	void update(double deltaTime) override;
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
	~MenuScene();
	void showOptions();
	void hideOptions();
private:
	OptionsScene *optionsScene;
	bool showingOptions = false;
};
