#include "OptionsScene.h"
#include "MenuScene.h"
#include "GLFW/glfw3.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiCheckbox.h"
#include <string>
#include "Ui/UiSliderInt.h"
#include "Render/PostProcessingShader.h"
#include "Ui/UiSlider.h"

OptionsScene::OptionsScene(MenuScene* menuScene) {
	this->menuScene = menuScene;
	textRenderer = assetManager->getTextRenderer();
	const float heightSeg = windowHeight / 18.0f;
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

	PostProcessingShader *pps = dynamic_cast<PostProcessingShader*>(assetManager->getShader(STPostProcessing));

	UiCheckbox *useHdr = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 8 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isHdrEnabled(), true);
	useHdr->setCheckboxCallback([pps](bool enabled) {
		pps->setHdr(enabled);
	});
	UiText *exposureText = new UiText(glm::vec2(windowCenterX, 9 * heightSeg), glm::vec2(windowWidth, heightSeg),"Exposure: " + std::to_string(pps->getExposure()));
	UiSlider *exposureSlider = new UiSlider(glm::vec2(windowCenterX, 10 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getExposure(), 0.0f, 10.0f);
	exposureSlider->setCallback([pps, exposureText](float exposure) {
		pps->setExposure(exposure);
		exposureText->setText("Exposure: " + std::to_string(exposure));
	});

	UiText *gammaText = new UiText(glm::vec2(windowCenterX, 11 * heightSeg), glm::vec2(windowWidth, heightSeg), "Gamma: " + std::to_string(pps->getGamma()));
	UiSlider *gammaSlider = new UiSlider(glm::vec2(windowCenterX, 12 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getGamma(), 0.0f, 10.0f);
	gammaSlider->setCallback([pps, gammaText](float gamma) {
		pps->setGamma(gamma);
		gammaText->setText("Gamma: " + std::to_string(gamma));
	});

	UiCheckbox *useBloom = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 13 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isBloomEnabled(), true);
	useBloom->setCheckboxCallback([pps](bool enabled) {
		pps->setBloom(enabled);
	});

	UiTextButton *back = new UiTextButton(glm::vec2(windowCenterX, 17 * heightSeg), "Back to menu");
	back->setButtonCallback([menuScene]() { menuScene->hideOptions(); });
	uiElements.emplace_back(back);
	uiElements.emplace_back(useLight);
	uiElements.emplace_back(castShadows);
	uiElements.emplace_back(texelSlider);
	uiElements.emplace_back(texelText);
	uiElements.emplace_back(samplesSlider);
	uiElements.emplace_back(samplesText);
	uiElements.emplace_back(useHdr);
	uiElements.emplace_back(exposureText);
	uiElements.emplace_back(exposureSlider);
	uiElements.emplace_back(gammaText);
	uiElements.emplace_back(gammaSlider);
	uiElements.emplace_back(useBloom);
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 2 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Cast shadows"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 3 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use light"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 8 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use HDR"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 13 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use bloom"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 0.5f * heightSeg), glm::vec2(windowWidth, 1.5f * heightSeg), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
}

void OptionsScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			menuScene->hideOptions();
		}
	}
}
