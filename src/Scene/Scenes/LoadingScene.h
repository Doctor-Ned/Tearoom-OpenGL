#pragma once
#include "Scene.h"

class UiText;

class LoadingScene : public Scene {
public:
	LoadingScene();
	void setLoadingText(std::string text);
private:
	UiText *loadingText;
};
