#include "OptionsScene.h"
#include "MenuScene.h"
#include "GLFW/glfw3.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiCheckbox.h"

OptionsScene::OptionsScene(MenuScene* menuScene) {
	this->menuScene = menuScene;
	textRenderer = sceneManager->getTextRenderer();
	const float checkboxShift = windowWidth / 8.0f;
	UiCheckbox *useLight = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 4 * windowHeight / 9.0f), sceneManager->useLight, true);
	useLight->setCheckboxCallback([&manager = sceneManager](bool useLight) { manager->useLight = useLight; });
	UiCheckbox *castShadows = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 5 * windowHeight / 9.0f), sceneManager->castShadows, true);
	castShadows->setCheckboxCallback([&manager = sceneManager](bool castShadows) { manager->castShadows = castShadows; });
	UiTextButton *back = new UiTextButton(glm::vec2(windowCenterX, 8 * windowHeight / 9.0f), "Back to menu");
	back->setButtonCallback([menuScene]() { menuScene->hideOptions(); });
	uiElements.emplace_back(back);
	uiElements.emplace_back(useLight);
	uiElements.emplace_back(castShadows);
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 4 * windowHeight / 9.0f), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use light"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 5 * windowHeight / 9.0f), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Cast shadows"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 1.0f * windowHeight / 9.0f), glm::vec2(windowWidth, 2.0f * windowHeight / 9.0f), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
}

void OptionsScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			menuScene->hideOptions();
		}
	}
}
