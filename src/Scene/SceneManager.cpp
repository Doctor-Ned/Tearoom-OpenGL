#include "SceneManager.h"

SceneManager& SceneManager::getInstance() {
	static SceneManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new SceneManager();
		instance->setup();
	}
	return *instance;
}

void SceneManager::setCurrentScene(Scene * scene)
{
	currentScene = scene;
}

void SceneManager::setup() {
	uiTextureShader = new Shader("Ui/uiTextureVS.glsl", "Ui/uiTextureFS.glsl");
	uiColorShader = new Shader("Ui/uiColorVS.glsl", "Ui/uiColorFS.glsl");
	skyboxShader = new Shader("skyboxVS.glsl", "skyboxFS.glsl");
	modelShader = new Shader("modelVS.glsl", "modelFS.glsl");
	textureShader = new Shader("textureVS.glsl", "textureFS.glsl");
	colorShader = new Shader("colorVS.glsl", "colorFS.glsl");
	reflectShader = new Shader("Ref/reflectVS.glsl", "Ref/reflectFS.glsl");
	refractShader = new Shader("Ref/refractVS.glsl", "Ref/refractFS.glsl");
	depthShader = new Shader("Depth/depthVS.glsl", "Depth/depthFS.glsl");
	depthPointShader = new GeometryShader("Depth/depthPointVS.glsl", "Depth/depthPointGS.glsl", "Depth/depthPointFS.glsl");
	depthDebugShader = new Shader("Depth/depthDebugVS.glsl", "Depth/depthDebugFS.glsl");
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
	textRenderer->load("res/fonts/ButterLayer.ttf", 60.0f);
}

void SceneManager::render() {
	if (currentScene != nullptr) {
		currentScene->render();
	}
}

void SceneManager::update(double timeDelta) {
	if (currentScene != nullptr) {
		currentScene->update(timeDelta);
	}
}

TextRenderer* SceneManager::getTextRenderer() {
	return textRenderer;
}

void SceneManager::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (currentScene != nullptr) {
		currentScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void SceneManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (currentScene != nullptr) {
		currentScene->mouse_callback(window, xpos, ypos);
	}
}

void SceneManager::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	if (currentScene != nullptr) {
		currentScene->mouse_button_callback(window, butt, action, mods);
	}
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
