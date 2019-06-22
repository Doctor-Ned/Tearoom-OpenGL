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
	resumeButton->addClickCallback([&]() {
		returnScene->showPauseScene = false;
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
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 1.0f * UI_REF_HEIGHT / 9.0f), glm::vec2(UI_REF_WIDTH, 2.0f * UI_REF_HEIGHT / 9.0f), "GAME PAUSED", MENU_TEXT_COLOR, MatchHeight));

	reinitializeRenderMap();
}
