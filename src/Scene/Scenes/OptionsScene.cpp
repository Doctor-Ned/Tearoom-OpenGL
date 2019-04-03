#include "OptionsScene.h"
#include "MenuScene.h"
#include "GLFW/glfw3.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiCheckbox.h"
#include <string>
#include "Ui/UiSliderInt.h"

OptionsScene::OptionsScene(MenuScene* menuScene) {
	this->menuScene = menuScene;
	textRenderer = assetManager->getTextRenderer();
	const float checkboxShift = windowWidth / 8.0f;
	UiCheckbox *useLight = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 2 * windowHeight / 9.0f), gameManager->useLight, true);
	useLight->setCheckboxCallback([&manager = gameManager](bool useLight) { manager->useLight = useLight; });
	UiCheckbox *castShadows = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 3 * windowHeight / 9.0f), gameManager->castShadows, true);
	castShadows->setCheckboxCallback([&manager = gameManager](bool castShadows) { manager->castShadows = castShadows; });
	UiText *texelText = new UiText(glm::vec2(windowCenterX, 4 * windowHeight / 9.0f), glm::vec2(windowWidth, windowHeight / 9.0f),
		"Texel resolution: " + std::to_string(gameManager->spotDirShadowTexelResolution));
	UiSliderInt *texelSlider = new UiSliderInt(glm::vec2(windowCenterX, 5 * windowHeight / 9.0f), glm::vec2(windowWidth / 2.0f, windowHeight / 9.0f), windowHeight / 9.0f / 2.0f,
		gameManager->spotDirShadowTexelResolution / 3, 0, 3);
	texelSlider->setCallback([&manager = gameManager, texelText](int power) {
		manager->spotDirShadowTexelResolution = static_cast<int>(pow(3, power));
		texelText->setText("Texel resolution: " + std::to_string(manager->spotDirShadowTexelResolution));
	});
	UiText *samplesText = new UiText(glm::vec2(windowCenterX, 6 * windowHeight / 9.0f), glm::vec2(windowWidth, windowHeight / 9.0f),
		"Point shadow samples: " + std::to_string(gameManager->pointShadowSamples));
	UiSliderInt *samplesSlider = new UiSliderInt(glm::vec2(windowCenterX, 7 * windowHeight / 9.0f), glm::vec2(windowWidth / 2.0f, windowHeight / 9.0f), windowHeight / 9.0f / 2.0f,
		gameManager->pointShadowSamples, 1, 250);
	samplesSlider->setCallback([&manager = gameManager, samplesText](int samples) {
		manager->pointShadowSamples = samples;
		samplesText->setText("Point shadow samples: " + std::to_string(manager->pointShadowSamples));
	});
	UiTextButton *back = new UiTextButton(glm::vec2(windowCenterX, 8 * windowHeight / 9.0f), "Back to menu");
	back->setButtonCallback([menuScene]() { menuScene->hideOptions(); });
	uiElements.emplace_back(back);
	uiElements.emplace_back(useLight);
	uiElements.emplace_back(castShadows);
	uiElements.emplace_back(texelSlider);
	uiElements.emplace_back(texelText);
	uiElements.emplace_back(samplesSlider);
	uiElements.emplace_back(samplesText);
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 2 * windowHeight / 9.0f), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Cast shadows"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 3 * windowHeight / 9.0f), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use light"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 1.0f * windowHeight / 9.0f), glm::vec2(windowWidth, 2.0f * windowHeight / 9.0f), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
}

void OptionsScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			menuScene->hideOptions();
		}
	}
}
