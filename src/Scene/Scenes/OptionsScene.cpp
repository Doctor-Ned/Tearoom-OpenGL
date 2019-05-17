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

VideoSettings OptionsScene::videoSettings{};

OptionsScene::OptionsScene(MenuScene* menuScene) {
	pps = dynamic_cast<PostProcessingShader*>(assetManager->getShader(STPostProcessing));
	this->menuScene = menuScene;
	textRenderer = assetManager->getTextRenderer();
	load();

	const float heightSeg = UI_REF_HEIGHT / 18.0f;
	const float checkboxShift = UI_REF_WIDTH / 8.0f;

	UiCanvas *screenTab = new UiCanvas(glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT));
	UiCanvas *generalTab = new UiCanvas(glm::vec2(0.0f, 0.0f), glm::vec2(UI_REF_WIDTH, UI_REF_HEIGHT));

	tabs.push_back(generalTab);
	tabs.push_back(screenTab);
	for (auto &tab : tabs) {
		rootUiElement->addChild(tab);
	}

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

	generalTab->addChild(enableLights);
	generalTab->addChild(enableShadowCasting);
	generalTab->addChild(texelSlider);
	generalTab->addChild(texelText);
	generalTab->addChild(samplesSlider);
	generalTab->addChild(samplesText);
	generalTab->addChild(useHdr);
	generalTab->addChild(exposureText);
	generalTab->addChild(exposureSlider);
	generalTab->addChild(gammaText);
	generalTab->addChild(gammaSlider);
	generalTab->addChild(useBloom);
	generalTab->addChild(enableVsync);
	generalTab->addChild(enableAntialiasing);
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 2 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable lights"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 3 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable shadow casting"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 8 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Use HDR"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 13 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Use bloom"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 14 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable VSync"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 15 * heightSeg), glm::vec2(2.0f*checkboxShift, BASE_BTN_SIZE*UI_REF_WIDTH), "Enable FXAA"));
	generalTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 0.5f * heightSeg), glm::vec2(UI_REF_WIDTH, 1.5f * heightSeg), "GENERAL SETTINGS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));

	UiButton *prevWindowType = new UiButton("res/ui/ButtonArrowLeftIdle.png", "res/ui/ButtonArrowLeftHover.png", "res/ui/ButtonArrowLeftClicked.png", glm::vec2(UI_REF_CEN_X * 0.75f - heightSeg, 3 * heightSeg), glm::vec2(heightSeg, heightSeg));
	UiButton *nextWindowType = new UiButton("res/ui/ButtonArrowIdle.png", "res/ui/ButtonArrowHover.png", "res/ui/ButtonArrowClicked.png", glm::vec2(UI_REF_CEN_X * 1.25f + heightSeg, 3 * heightSeg), glm::vec2(heightSeg, heightSeg));
	UiText *windowTypeText = new UiText(glm::vec2(UI_REF_CEN_X, 3 * heightSeg), glm::vec2(UI_REF_CEN_X / 2.0f, 2 * heightSeg), WindowTypeNames[videoSettings.windowType]);
	prevWindowType->addClickCallback([windowTypeText]() {
		if (static_cast<int>(videoSettings.windowType) == 0) {
			videoSettings.windowType = WindowTypes[(sizeof(WindowTypes) / sizeof(*WindowTypes)) - 1];
		} else {
			videoSettings.windowType = static_cast<WindowType>(static_cast<int>(videoSettings.windowType) - 1);
		}
		windowTypeText->setText(WindowTypeNames[videoSettings.windowType]);
	});

	nextWindowType->addClickCallback([windowTypeText]() {
		if (static_cast<int>(videoSettings.windowType) == (sizeof(WindowTypes) / sizeof(*WindowTypes)) - 1) {
			videoSettings.windowType = WindowTypes[0];
		} else {
			videoSettings.windowType = static_cast<WindowType>(static_cast<int>(videoSettings.windowType) + 1);
		}
		windowTypeText->setText(WindowTypeNames[videoSettings.windowType]);
	});

	UiButton *applyButton = new UiTextButton(glm::vec2(UI_REF_CEN_X, 15 * heightSeg), "Apply (restart needed)");
	applyButton->addClickCallback([this]() {
		saveVideoSettings();
	});

	screenTab->addChild(prevWindowType);
	screenTab->addChild(nextWindowType);
	screenTab->addChild(windowTypeText);
	screenTab->addChild(applyButton);
	screenTab->addChild(new UiText(glm::vec2(UI_REF_CEN_X, 0.5f * heightSeg), glm::vec2(UI_REF_WIDTH, 1.5f * heightSeg), "SCREEN SETTINGS", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight));

	UiTextButton *back = new UiTextButton(glm::vec2(UI_REF_CEN_X, 17 * heightSeg), "Back to menu");
	back->addClickCallback([this, menuScene]() { save(); menuScene->hideOptions(); });

	glm::vec2 arrowSize(back->getSize().y, back->getSize().y);
	glm::vec2 arrowLeftPos(back->getPosition().x - arrowSize.x / 2.0f, back->getPosition().y);
	glm::vec2 arrowRightPos(arrowLeftPos.x + back->getSize().x + arrowSize.x, arrowLeftPos.y);;

	arrowLeft = new UiButton("res/ui/ButtonArrowLeftIdle.png", "res/ui/ButtonArrowLeftHover.png", "res/ui/ButtonArrowLeftClicked.png", arrowLeftPos, arrowSize, Top);
	arrowLeft->addClickCallback([this]() {prevTab(); });
	arrowRight = new UiButton("res/ui/ButtonArrowIdle.png", "res/ui/ButtonArrowHover.png", "res/ui/ButtonArrowClicked.png", arrowRightPos, arrowSize, Top);
	arrowRight->addClickCallback([this]() {nextTab(); });

	rootUiElement->addChild(arrowLeft);
	rootUiElement->addChild(arrowRight);
	rootUiElement->addChild(back);

	refreshTab();

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

VideoSettings OptionsScene::loadVideoSettings() {
	videoSettings = DataSerializer::deserializeVideoSettings(Global::readJsonFile(VIDEO_SETTINGS_FILE));
	return videoSettings;
}

void OptionsScene::setVideoSettings(VideoSettings videoSettings) {
	OptionsScene::videoSettings = videoSettings;
}

void OptionsScene::saveVideoSettings() {
	Global::saveToFile(VIDEO_SETTINGS_FILE, DataSerializer::serializeVideoSettings(videoSettings));
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

void OptionsScene::refreshTab() {
	for (int i = 0; i < tabs.size(); i++) {
		tabs[i]->setActive(i == currTab);
	}
	arrowLeft->setActive(currTab > 0);
	arrowRight->setActive(currTab < tabs.size() - 1);
}

void OptionsScene::prevTab() {
	if (currTab > 0) {
		currTab--;
		refreshTab();
	}
}

void OptionsScene::nextTab() {
	if (currTab < tabs.size() - 1) {
		currTab++;
		refreshTab();
	}
}
