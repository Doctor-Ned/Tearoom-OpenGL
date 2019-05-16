#pragma once
#include "Scene.h"

class UiText;
class UiColorPlane;

class LoadingScene : public Scene {
public:
	LoadingScene();
	void setLoadingText(std::string text);
	void update(double deltaTime) override;
private:
	UiText *loadingText;
	UiColorPlane *loadingBar;
	float loadingBarWidth;
};
