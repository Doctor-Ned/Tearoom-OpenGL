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
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
	static VideoSettings loadVideoSettings();
	static void setVideoSettings(VideoSettings videoSettings);
	static void saveVideoSettings();
private:
	static VideoSettings videoSettings;
	void addResolution(int width, int height);
	void sortResolutions();
	void refreshCurrentResolution();
	static std::string resolutionToString(std::pair<int, int> res);
	int currResolution = 0;
	bool compareResolutions(std::pair<int, int> a, std::pair<int, int> b);
	void swap(std::pair<int, int> &a, std::pair<int, int> &b);
	std::vector<std::pair<int, int>> resolutions;
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
