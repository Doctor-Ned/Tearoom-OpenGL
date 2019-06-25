#include "PauseScene.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiColorPlane.h"

PauseScene::PauseScene(Scene * returnScene) {
	if(returnScene == nullptr) {
		SPDLOG_DEBUG("Provided return scene is null and should not ever be!");
		throw std::exception("Provided return scene is null and should not ever be!");
	}
	UiColorPlane *background = new UiColorPlane(glm::vec4(0.0f,0.0f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT), TopLeft);
	UiTextButton *resumeButton = new UiTextButton(glm::vec2(UI_REF_CEN_X, 4 * UI_REF_HEIGHT / 9.0f), "Resume");
	resumeButton->addClickCallback([=]() {
		returnScene->showPauseScene = false;
		returnScene->setCursorLocked(returnScene->wasMouseLocked);
	});
	UiTextButton *optionsButton = new UiTextButton(glm::vec2(UI_REF_CEN_X, 5 * UI_REF_HEIGHT / 9.0f), "Options");
	optionsButton->addClickCallback([&]() {
		gameManager->goToOptions();
	});
	UiTextButton *menuButton = new UiTextButton(glm::vec2(UI_REF_CEN_X, 6 * UI_REF_HEIGHT / 9.0f), "Exit to menu");
	menuButton->addClickCallback([&]() {
		gameManager->goToMenu();
	});
	rootUiElement->addChild(background);
	rootUiElement->addChild(resumeButton);
	rootUiElement->addChild(optionsButton);
	rootUiElement->addChild(menuButton);
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 1.0f * UI_REF_HEIGHT / 9.0f), glm::vec2(UI_REF_WIDTH, 2.0f * UI_REF_HEIGHT / 9.0f), "GAME PAUSED", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 8.0f * UI_REF_HEIGHT / 9.0f), glm::vec2(UI_REF_WIDTH * 0.7f, 2.0f * UI_REF_HEIGHT / 9.0f), "Press F4 for control hints", glm::vec3(1.0f, 1.0f, 1.0f), MatchWidth));

	reinitializeRenderMap();
}
