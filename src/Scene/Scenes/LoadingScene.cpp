#include "LoadingScene.h"
#include "Ui/UiText.h"
#include "Ui/UiColorPlane.h"

LoadingScene::LoadingScene() {
	UiColorPlane *background = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0, 0), glm::vec2(windowWidth, windowHeight), false);
	loadingText = new UiText(glm::vec2(windowCenterX, windowCenterY), glm::vec2(windowWidth * 0.75f, windowHeight), "Loading...", glm::vec3(1.0f, 1.0f, 1.0f), MatchWidth);
	uiElements.emplace_back(background);
	uiElements.emplace_back(loadingText);
	reinitializeRenderMap();
}
void LoadingScene::setLoadingText(std::string text) {
	loadingText->setText(text);
}