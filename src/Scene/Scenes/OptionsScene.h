#pragma once
#include "Scene.h"
#include "Render/TextRenderer.h"
#include "Render/PostProcessingShader.h"

#define VIDEO_SETTINGS_FILE "videoSettings.json"

class UiButton;
class MenuScene;

class OptionsScene : public Scene {
public:
	OptionsScene(MenuScene *menuScene);
	~OptionsScene();
	void update(double deltaTime) override;
	void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	static VideoSettings loadVideoSettings();
	static void setVideoSettings(VideoSettings videoSettings);
	static void saveVideoSettings();
private:
	static VideoSettings videoSettings;
	void load();
	void save();
	void refreshTab();
	void prevTab();
	void nextTab();
	int currTab = 0;
	std::vector<UiCanvas*> tabs;
	UiButton *arrowLeft, *arrowRight;
	TextRenderer *textRenderer;
	MenuScene *menuScene;
	const char* SETTINGS_FILE = "settings.json";
	PostProcessingShader* pps;
};
