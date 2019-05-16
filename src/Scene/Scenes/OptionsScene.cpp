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
	const float heightSeg = UI_REF_HEIGHT / 18.0f;
	const float checkboxShift = UI_REF_WIDTH / 8.0f;
	UiCheckbox *enableLights = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 2 * heightSeg), glm::vec2(heightSeg, heightSeg), lightManager->enableLights, Center);
	enableLights->setCheckboxCallback([&manager = lightManager](bool enableLights) { manager->enableLights = enableLights; });
	//enableLights->setRotationAnchor(TopLeft);
	UiCheckbox *enableShadowCasting = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 3 * heightSeg), glm::vec2(heightSeg, heightSeg), lightManager->enableShadowCasting, Center);
	enableShadowCasting->setCheckboxCallback([&manager = lightManager](bool enableShadowCasting) { manager->enableShadowCasting = enableShadowCasting; });
	UiText *texelText = new UiText(glm::vec2(UI_REF_CEN_X, 4 * heightSeg), glm::vec2(UI_REF_WIDTH, heightSeg),
		"Texel resolution: " + std::to_string(lightManager->spotDirShadowTexelResolution));
	UiSliderInt *texelSlider = new UiSliderInt(glm::vec2(UI_REF_CEN_X, 5 * heightSeg), glm::vec2(UI_REF_WIDTH / 2.0f, heightSeg), heightSeg / 2.0f,
		lightManager->spotDirShadowTexelResolution / 3, 0, 3);
	texelSlider->setCallback([&manager = lightManager, texelText](int power) {
		manager->spotDirShadowTexelResolution = static_cast<int>(pow(3, power));
		texelText->setText("Texel resolution: " + std::to_string(manager->spotDirShadowTexelResolution));
	});
	UiText *samplesText = new UiText(glm::vec2(UI_REF_CEN_X, 6 * heightSeg), glm::vec2(UI_REF_WIDTH, heightSeg),
		"Point shadow samples: " + std::to_string(lightManager->pointShadowSamples));
	UiSliderInt *samplesSlider = new UiSliderInt(glm::vec2(UI_REF_CEN_X, 7 * heightSeg), glm::vec2(UI_REF_WIDTH / 2.0f, heightSeg), heightSeg / 2.0f,
		lightManager->pointShadowSamples, 1, 250);
	samplesSlider->setCallback([&manager = lightManager, samplesText](int samples) {
		manager->pointShadowSamples = samples;
		samplesText->setText("Point shadow samples: " + std::to_string(manager->pointShadowSamples));
	});

	UiCheckbox *useHdr = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 8 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isHdrEnabled(), Center);
	useHdr->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setHdr(enabled);
	});
	UiText *exposureText = new UiText(glm::vec2(UI_REF_CEN_X, 9 * heightSeg), glm::vec2(UI_REF_WIDTH, heightSeg), "Exposure: " + std::to_string(pps->getExposure()));
	UiSlider *exposureSlider = new UiSlider(glm::vec2(UI_REF_CEN_X, 10 * heightSeg), glm::vec2(UI_REF_WIDTH / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getExposure(), 0.0f, 10.0f);
	exposureSlider->setCallback([pps = pps, exposureText](float exposure) {
		pps->use();
		pps->setExposure(exposure);
		exposureText->setText("Exposure: " + std::to_string(exposure));
	});

	UiText *gammaText = new UiText(glm::vec2(UI_REF_CEN_X, 11 * heightSeg), glm::vec2(UI_REF_WIDTH, heightSeg), "Gamma: " + std::to_string(pps->getGamma()));
	UiSlider *gammaSlider = new UiSlider(glm::vec2(UI_REF_CEN_X, 12 * heightSeg), glm::vec2(UI_REF_WIDTH / 2.0f, heightSeg), heightSeg / 2.0f,
		pps->getGamma(), 0.0f, 10.0f);
	gammaSlider->setCallback([pps = pps, gammaText](float gamma) {
		pps->use();
		pps->setGamma(gamma);
		gammaText->setText("Gamma: " + std::to_string(gamma));
	});

	UiCheckbox *useBloom = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 13 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isBloomEnabled(), Center);
	useBloom->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setBloom(enabled);
	});

	UiCheckbox *enableVsync = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 14 * heightSeg), glm::vec2(heightSeg, heightSeg), gameManager->isVsyncEnabled(), Center);
	enableVsync->setCheckboxCallback([gameManager = gameManager](bool enabled) {
		gameManager->setVsync(enabled);
	});

	UiCheckbox *enableAntialiasing = new UiCheckbox(glm::vec2(UI_REF_CEN_X - checkboxShift, 15 * heightSeg), glm::vec2(heightSeg, heightSeg), pps->isAntialiasingEnabled(), Center);
	enableAntialiasing->setCheckboxCallback([pps = pps](bool enabled) {
		pps->use();
		pps->setAntialiasing(enabled);
	});

	UiTextButton *back = new UiTextButton(glm::vec2(UI_REF_CEN_X, 17 * heightSeg), "Back to menu");
	back->addClickCallback([menuScene]() { menuScene->hideOptions(); });
	rootUiElement->addChild(back);
	rootUiElement->addChild(enableLights);
	rootUiElement->addChild(enableShadowCasting);
	rootUiElement->addChild(texelSlider);
	rootUiElement->addChild(texelText);
	rootUiElement->addChild(samplesSlider);
	rootUiElement->addChild(samplesText);
	rootUiElement->addChild(useHdr);
	rootUiElement->addChild(exposureText);
	rootUiElement->addChild(exposureSlider);
	rootUiElement->addChild(gammaText);
	rootUiElement->addChild(gammaSlider);
	rootUiElement->addChild(useBloom);
	rootUiElement->addChild(enableVsync);
	rootUiElement->addChild(enableAntialiasing);
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 2 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable lights"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 3 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable shadow casting"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 8 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Use HDR"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 13 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Use bloom"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 14 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable VSync"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 15 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable FXAA"));
	rootUiElement->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 0.5f * heightSeg), glm::vec2(UI_REF_WIDTH, 1.5f * heightSeg), "OPTIONS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));
	reinitializeRenderMap();
}

OptionsScene::~OptionsScene() {
	save();
}

void OptionsScene::update(double deltaTime) {
	//rootUiElement->getChildren()[1]->localTransform.rotateZDegrees(30.0f*deltaTime);
	//rootUiElement->getChildren()[3]->localTransform.rotateZDegrees(40.0f*deltaTime);
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
	lightManager->enableLights = root.get("enableLights", lightManager->enableLights).asBool();
	lightManager->enableShadowCasting = root.get("enableShadowCasting", lightManager->enableShadowCasting).asBool();
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
	root["enableLights"] = lightManager->enableLights;
	root["enableShadowCasting"] = lightManager->enableShadowCasting;
	root["useVsync"] = gameManager->isVsyncEnabled();
	root["exposure"] = pps->getExposure();
	root["gamma"] = pps->getGamma();
	root["useAntialiasing"] = pps->isAntialiasingEnabled();
	root["useHdr"] = pps->isHdrEnabled();
	root["useBloom"] = pps->isBloomEnabled();
	Global::saveToFile(SETTINGS_FILE, root);
}
