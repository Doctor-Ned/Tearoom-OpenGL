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
	const float heightSeg = windowHeight / 15.0f;
	const float checkboxShift = windowWidth / 8.0f;
	UiCheckbox *useLight = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 2 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->useLight, true);
	useLight->setCheckboxCallback([&manager = gameManager](bool useLight) { manager->useLight = useLight; });
	UiCheckbox *castShadows = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 3 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->castShadows, true);
	castShadows->setCheckboxCallback([&manager = gameManager](bool castShadows) { manager->castShadows = castShadows; });
	UiText *texelText = new UiText(glm::vec2(windowCenterX, 4 * heightSeg), glm::vec2(windowWidth, heightSeg),
		"Texel resolution: " + std::to_string(gameManager->spotDirShadowTexelResolution));
	UiSliderInt *texelSlider = new UiSliderInt(glm::vec2(windowCenterX, 5 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		gameManager->spotDirShadowTexelResolution / 3, 0, 3);
	texelSlider->setCallback([&manager = gameManager, texelText](int power) {
		manager->spotDirShadowTexelResolution = static_cast<int>(pow(3, power));
		texelText->setText("Texel resolution: " + std::to_string(manager->spotDirShadowTexelResolution));
	});
	UiText *samplesText = new UiText(glm::vec2(windowCenterX, 6 * heightSeg), glm::vec2(windowWidth, heightSeg),
		"Point shadow samples: " + std::to_string(gameManager->pointShadowSamples));
	UiSliderInt *samplesSlider = new UiSliderInt(glm::vec2(windowCenterX, 7 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		gameManager->pointShadowSamples, 1, 250);
	samplesSlider->setCallback([&manager = gameManager, samplesText](int samples) {
		manager->pointShadowSamples = samples;
		samplesText->setText("Point shadow samples: " + std::to_string(manager->pointShadowSamples));
	});
	UiTextButton *back = new UiTextButton(glm::vec2(windowCenterX, 14 * heightSeg), "Back to menu");
	back->setButtonCallback([menuScene]() { menuScene->hideOptions(); });
	uiElements.emplace_back(back);
	uiElements.emplace_back(useLight);
	uiElements.emplace_back(castShadows);
	uiElements.emplace_back(texelSlider);
	uiElements.emplace_back(texelText);
	uiElements.emplace_back(samplesSlider);
	uiElements.emplace_back(samplesText);
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 2 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Cast shadows"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 3 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use light"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 0.5f * heightSeg), glm::vec2(windowWidth, 2.0f * heightSeg), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
}

void OptionsScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			menuScene->hideOptions();
		}
	}
}
