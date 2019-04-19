#include "OptionsScene.h"
#include "MenuScene.h"
#include "GLFW/glfw3.h"
#include "Ui/UiTextButton.h"
#include "Ui/UiCheckbox.h"
#include <string>
#include "Ui/UiSliderInt.h"
#include "Render/PostProcessingShader.h"
#include "Ui/UiSlider.h"
#include "Render/LightManager.h"

OptionsScene::OptionsScene(MenuScene* menuScene) {
	pps = dynamic_cast<PostProcessingShader*>(assetManager->getShader(STPostProcessing));
	this->menuScene = menuScene;
	textRenderer = assetManager->getTextRenderer();
	load();
	const float heightSeg = windowHeight / 18.0f;
	const float checkboxShift = windowWidth / 8.0f;
	UiCheckbox *useLight = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 2 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->useLight, true);
	useLight->setCheckboxCallback([&manager = gameManager](bool useLight) { manager->useLight = useLight; });
	UiCheckbox *castShadows = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 3 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->castShadows, true);
	castShadows->setCheckboxCallback([&manager = gameManager](bool castShadows) { manager->castShadows = castShadows; });
	UiText *texelText = new UiText(glm::vec2(windowCenterX, 4 * heightSeg), glm::vec2(windowWidth, heightSeg),
		"Texel resolution: " + std::to_string(lightManager->spotDirShadowTexelResolution));
	UiSliderInt *texelSlider = new UiSliderInt(glm::vec2(windowCenterX, 5 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		lightManager->spotDirShadowTexelResolution / 3, 0, 3);
	texelSlider->setCallback([&manager = lightManager, texelText](int power) {
		manager->spotDirShadowTexelResolution = static_cast<int>(pow(3, power));
		texelText->setText("Texel resolution: " + std::to_string(manager->spotDirShadowTexelResolution));
	});
	UiText *samplesText = new UiText(glm::vec2(windowCenterX, 6 * heightSeg), glm::vec2(windowWidth, heightSeg),
		"Point shadow samples: " + std::to_string(lightManager->pointShadowSamples));
	UiSliderInt *samplesSlider = new UiSliderInt(glm::vec2(windowCenterX, 7 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		lightManager->pointShadowSamples, 1, 250);
	samplesSlider->setCallback([&manager = lightManager, samplesText](int samples) {
		manager->pointShadowSamples = samples;
		samplesText->setText("Point shadow samples: " + std::to_string(manager->pointShadowSamples));
	});

	UiCheckbox *useHdr = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 8 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isHdrEnabled(), true);
	useHdr->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setHdr(enabled);
	});
	UiText *exposureText = new UiText(glm::vec2(windowCenterX, 9 * heightSeg), glm::vec2(windowWidth, heightSeg), "Exposure: " + std::to_string(pps->getExposure()));
	UiSlider *exposureSlider = new UiSlider(glm::vec2(windowCenterX, 10 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getExposure(), 0.0f, 10.0f);
	exposureSlider->setCallback([pps = pps, exposureText](float exposure) {
		pps->use();
		pps->setExposure(exposure);
		exposureText->setText("Exposure: " + std::to_string(exposure));
	});

	UiText *gammaText = new UiText(glm::vec2(windowCenterX, 11 * heightSeg), glm::vec2(windowWidth, heightSeg), "Gamma: " + std::to_string(pps->getGamma()));
	UiSlider *gammaSlider = new UiSlider(glm::vec2(windowCenterX, 12 * heightSeg), glm::vec2(windowWidth / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getGamma(), 0.0f, 10.0f);
	gammaSlider->setCallback([pps = pps, gammaText](float gamma) {
		pps->use();
		pps->setGamma(gamma);
		gammaText->setText("Gamma: " + std::to_string(gamma));
	});

	UiCheckbox *useBloom = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 13 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isBloomEnabled(), true);
	useBloom->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setBloom(enabled);
	});

	UiCheckbox *enableVsync = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 14 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->isVsyncEnabled(), true);
	enableVsync->setCheckboxCallback([gameManager = gameManager](bool enabled) {
		gameManager->setVsync(enabled);
	});

	UiCheckbox *enableAntialiasing = new UiCheckbox(glm::vec2(windowCenterX - checkboxShift, 15 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isAntialiasingEnabled(), true);
	enableAntialiasing->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setAntialiasing(enabled);
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
	uiElements.emplace_back(enableVsync);
	uiElements.emplace_back(enableAntialiasing);
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 2 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Cast shadows"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 3 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use light"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 8 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use HDR"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 13 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Use bloom"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 14 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Enable VSync"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 15 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*windowWidth), "Enable FXAA"));
	uiElements.emplace_back(new UiText(glm::vec2(windowCenterX, 0.5f * heightSeg), glm::vec2(windowWidth, 1.5f * heightSeg), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
}

OptionsScene::~OptionsScene() {
	save();
}

void OptionsScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			menuScene->hideOptions();
		}
	}
}

void OptionsScene::load() {
	Json::Value root = Global::readJsonFile(SETTINGS_FILE);
	lightManager->pointShadowSamples = root.get("pointShadowSamples", lightManager->pointShadowSamples).asInt();
	lightManager->spotDirShadowTexelResolution = root.get("spotDirShadowTexelResolution", lightManager->spotDirShadowTexelResolution).asInt();
	gameManager->useLight = root.get("useLight", gameManager->useLight).asBool();
	gameManager->castShadows = root.get("castShadows", gameManager->castShadows).asBool();
	gameManager->setVsync(root.get("useVsync", gameManager->isVsyncEnabled()).asBool());
	pps->use();
	pps->setExposure(root.get("exposure", pps->getExposure()).asFloat());
	pps->setGamma(root.get("gamma", pps->getGamma()).asFloat());
	pps->setAntialiasing(root.get("useAntialiasing", pps->isAntialiasingEnabled()).asBool());
	pps->setHdr(root.get("useHdr", pps->isHdrEnabled()).asBool());
	pps->setBloom(root.get("useBloom", pps->isBloomEnabled()).asBool());
}

void OptionsScene::save() {
	Json::Value root;
	root["pointShadowSamples"] = lightManager->pointShadowSamples;
	root["spotDirShadowTexelResolution"] = lightManager->spotDirShadowTexelResolution;
	root["useLight"] = gameManager->useLight;
	root["castShadows"] = gameManager->castShadows;
	root["useVsync"] = gameManager->isVsyncEnabled();
	root["exposure"] = pps->getExposure();
	root["gamma"] = pps->getGamma();
	root["useAntialiasing"] = pps->isAntialiasingEnabled();
	root["useHdr"] = pps->isHdrEnabled();
	root["useBloom"] = pps->isBloomEnabled();
	Global::saveToFile(SETTINGS_FILE, root);
}
