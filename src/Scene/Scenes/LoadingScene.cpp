#include "LoadingScene.h"
#include "Ui/UiText.h"
#include "Ui/UiColorPlane.h"

LoadingScene::LoadingScene() {
	UiColorPlane *background = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0, 0), glm::vec2(windowWidth, windowHeight), TopLeft);
	loadingText = new UiText(glm::vec2(windowCenterX, windowCenterY), glm::vec2(windowWidth * 0.75f, windowHeight), "Loading...", glm::vec3(1.0f, 1.0f, 1.0f), MatchWidth);
	loadingBarWidth = windowWidth * 0.75f;
	UiColorPlane *loadingBar1 = new UiColorPlane(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec2(windowCenterX - loadingBarWidth / 2.0f, windowHeight * 0.75f), glm::vec2(loadingBarWidth, windowHeight*0.15f), Left);
	loadingBar = new UiColorPlane(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(windowCenterX - loadingBarWidth / 2.0f, windowHeight * 0.75f), glm::vec2(loadingBarWidth, windowHeight*0.15f), Left);
	loadingBar->setRotationAnchor(Left);
	rootUiElement->addChild(background);
	rootUiElement->addChild(loadingText);
	rootUiElement->addChild(loadingBar1);
	rootUiElement->addChild(loadingBar);
	reinitializeRenderMap();
}

void LoadingScene::setLoadingText(std::string text) {
	loadingText->setText(text);
}

void LoadingScene::update(double deltaTime) {
	loadingBar->localTransform.setScale(glm::vec3(assetManager->getLoadingProgress(), 1.0f, 1.0f));
}
