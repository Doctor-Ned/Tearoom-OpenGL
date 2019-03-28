#include "SceneManager.h"

SceneManager& SceneManager::getInstance() {
	static SceneManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new SceneManager();
		instance->setup();
	}
	return *instance;
}

void SceneManager::setup() {
	uiTextureShader = new Shader("uiTextureVertexShader.glsl", "uiTextureFragmentShader.glsl");
	uiColorShader = new Shader("uiColorVertexShader.glsl", "uiColorFragmentShader.glsl");
	skyboxShader = new Shader("skyboxVertexShader.glsl", "skyboxFragmentShader.glsl");
	modelShader = new Shader("modelVertexShader.glsl", "modelFragmentShader.glsl");
	textureShader = new Shader("textureVertexShader.glsl", "textureFragmentShader.glsl");
	colorShader = new Shader("colorVertexShader.glsl", "colorFragmentShader.glsl");
	reflectShader = new Shader("reflectVertexShader.glsl", "reflectFragmentShader.glsl");
	refractShader = new Shader("refractVertexShader.glsl", "refractFragmentShader.glsl");
	depthShader = new Shader("depthVertexShader.glsl", "depthFragmentShader.glsl");
	depthPointShader = new GeometryShader("depthPointVertexShader.glsl", "depthPointGeometryShader.glsl", "depthPointFragmentShader.glsl");
	depthDebugShader = new Shader("depthDebugVertexShader.glsl", "depthDebugFragmentShader.glsl");
	uboLights = new UboLights(nullptr, nullptr, nullptr);
	uboTextureColor = new UboTextureColor(false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	uboViewProjection = new UboViewProjection(glm::mat4(1.0f), glm::mat4(1.0f));
	std::vector<Ubo*> ubos;
	ubos.push_back(uboLights);
	ubos.push_back(uboTextureColor);
	ubos.push_back(uboViewProjection);
	std::vector<Shader*> shaders;
	shaders.push_back(modelShader);
	shaders.push_back(textureShader);
	shaders.push_back(colorShader);

	reflectShader->bind(uboViewProjection);
	refractShader->bind(uboViewProjection);

	for (int i = 0; i < shaders.size(); i++) {
		for (int j = 0; j < ubos.size(); j++) {
			shaders[i]->bind(ubos[j]);
		}
	}

	textRenderer = new TextRenderer(0.5f);
	textRenderer->load("res\\fonts\\ButterLayer.ttf", 60.0f);
	mainMenuScene = new MainMenuScene();
	optionsScene = new OptionsScene();
	mainMenuScene->setOptionsCallback([this]() { currentScene = optionsScene; });
	currentScene = mainMenuScene;
}

void SceneManager::render() {
	currentScene->render();
}

void SceneManager::update(double timeDelta) {
	currentScene->update(timeDelta);
}

void SceneManager::newGame() {
	gameScene = new GameScene();
	currentScene = gameScene;
}

void SceneManager::backToMenu() {
	currentScene = mainMenuScene;
}

TextRenderer* SceneManager::getTextRenderer() {
	return textRenderer;
}

void SceneManager::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	currentScene->keyboard_callback(window, key, scancode, action, mods);
}

void SceneManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	currentScene->mouse_callback(window, xpos, ypos);
}

void SceneManager::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	currentScene->mouse_button_callback(window, butt, action, mods);
}

Shader* SceneManager::getUiColorShader() {
	return uiColorShader;
}

Shader* SceneManager::getUiTextureShader() {
	return uiTextureShader;
}

Shader* SceneManager::getSkyboxShader() {
	return skyboxShader;
}

Shader* SceneManager::getModelShader() {
	return modelShader;
}

Shader* SceneManager::getTextureShader() {
	return textureShader;
}

Shader* SceneManager::getColorShader() {
	return colorShader;
}

Shader* SceneManager::getReflectShader() {
	return reflectShader;
}

Shader* SceneManager::getRefractShader() {
	return refractShader;
}

Shader* SceneManager::getDepthShader() {
	return depthShader;
}

GeometryShader* SceneManager::getDepthPointShader() {
	return depthPointShader;
}

Shader* SceneManager::getDepthDebugShader() {
	return depthDebugShader;
}

UboLights* SceneManager::getUboLights() {
	return uboLights;
}

UboTextureColor* SceneManager::getUboTextureColor() {
	return uboTextureColor;
}

UboViewProjection* SceneManager::getUboViewProjection() {
	return uboViewProjection;
}

GLuint SceneManager::getFramebuffer() {
	return framebuffer;
}

void SceneManager::setFramebuffer(GLuint fbo) {
	framebuffer = fbo;
}
