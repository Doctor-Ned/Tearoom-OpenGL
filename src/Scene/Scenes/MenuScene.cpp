#include "MenuScene.h"
#include "MiszukScene.h"
#include "TestScene.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiPlane.h"
#include "Serialization/Serializer.h"
#include "EditorScene.h"
#include "Scene/SoundSystem.h"
#include "MenuPreview.h"

class UiTextButton;

MenuScene::MenuScene() {
	UiColorPlane *background = new UiColorPlane(MENU_BACKGROUND_COLOR, glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT), TopLeft);
	UiTextButton *miszukScene = new UiTextButton(glm::vec2(UI_REF_CEN_X, 3 * UI_REF_HEIGHT / 9.0f), "Miszuk scene");
	miszukScene->addClickCallback([]() {
		SoundSystem::getSound("bow")->play();
		GameManager::getInstance()->setCurrentScene(new MiszukScene());
	});
	UiTextButton *newTestScene = new UiTextButton(glm::vec2(UI_REF_CEN_X, 4 * UI_REF_HEIGHT / 9.0f), "New test scene");
	newTestScene->addClickCallback([]() {
		SoundSystem::getSound("bow")->play();
		GameManager::getInstance()->setCurrentScene(new TestScene());
	});
	UiTextButton *loadTestScene = new UiTextButton(glm::vec2(UI_REF_CEN_X, 5 * UI_REF_HEIGHT / 9.0f), "Load test scene");
	loadTestScene->addClickCallback([this]() {
		SoundSystem::getSound("bow")->play();
		Scene *scene = Serializer::getInstance()->loadScene("test");
		if (scene != nullptr) {
			gameManager->setCurrentScene(scene);
		}
	});
	UiTextButton *editorScene = new UiTextButton(glm::vec2(UI_REF_CEN_X, 6 * UI_REF_HEIGHT / 9.0f), "Editor scene");
	editorScene->addClickCallback([]() {
		SoundSystem::getSound("bow")->play();
		GameManager::getInstance()->setCurrentScene(new EditorScene());
	});
	UiTextButton *options = new UiTextButton(glm::vec2(UI_REF_CEN_X, 7 * UI_REF_HEIGHT / 9.0f), "Options");
	options->addClickCallback([this]() {
		SoundSystem::getSound("bow")->play();
		gameManager->goToOptions();
	});
	UiTextButton *quit = new UiTextButton(glm::vec2(UI_REF_CEN_X, 8 * UI_REF_HEIGHT / 9.0f), "Quit");
	quit->addClickCallback([]() {GameManager::getInstance()->quit(); });

	UiTextButton *menuPreview = new UiTextButton(glm::vec2(UI_REF_CEN_X * 1.7, 8 * UI_REF_HEIGHT / 9.0f), "Menu Preview");
	menuPreview->addClickCallback([this]() {
		SoundSystem::getSound("bow")->play();
		GameManager::getInstance()->goToMenu();
	});
	engineLogo = new UiPlane("res/textures/capybaraLogo.PNG", glm::vec2(220.0f, 590.0f), glm::vec2(300.0f, 150.0f), Center);
	rootUiElement->addChild(background);
	rootUiElement->addChild(engineLogo);
	rootUiElement->addChild(miszukScene);
	rootUiElement->addChild(newTestScene);
	rootUiElement->addChild(loadTestScene);
	rootUiElement->addChild(editorScene);
	rootUiElement->addChild(options);
	rootUiElement->addChild(quit);
	rootUiElement->addChild(menuPreview);
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 1.0f * UI_REF_HEIGHT / 9.0f), glm::vec2(UI_REF_WIDTH, 2.0f * UI_REF_HEIGHT / 9.0f), "MAIN MENU", MENU_TEXT_COLOR, MatchHeight));

	reinitializeRenderMap();
}