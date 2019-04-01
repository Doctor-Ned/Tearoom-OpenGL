#include "SceneManager.h"
#include "GLFW/glfw3.h"

SceneManager *SceneManager::getInstance() {
	static SceneManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new SceneManager();
	}
	return instance;
}

void SceneManager::setCurrentScene(Scene * scene) {
	currentScene = scene;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void SceneManager::setCurrentSceneAndDeletePrevious(Scene* scene) {
	Scene* old = currentScene;
	currentScene = scene;
	if (old != nullptr) {
		delete old;
	}
}

float SceneManager::getWindowWidth() {
	return windowWidth;
}

float SceneManager::getWindowHeight() {
	return windowHeight;
}

float SceneManager::getScreenWidth() {
	return screenWidth;
}

float SceneManager::getScreenHeight() {
	return screenHeight;
}

float SceneManager::getWindowCenterX() {
	return windowCenterX;
}

float SceneManager::getWindowCenterY() {
	return windowCenterY;
}

void SceneManager::setWindow(GLFWwindow* window) {
	this->window = window;
}

void SceneManager::setCursorLocked(bool locked) {
	glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void SceneManager::goToMenu(bool destroyPreviousScene) {
	Scene* old = currentScene;
	currentScene = menuScene;
	setCursorLocked(false);
	if (old != nullptr && old != menuScene && destroyPreviousScene) {
		delete old;
	}
}

Shader* SceneManager::getShader(ShaderType type) {
	if (type == STNone) {
		return nullptr;
	}
	return shaders.find(type)->second;
}

void SceneManager::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void SceneManager::setup() {
	shaders.emplace(STUiTexture, new Shader("Ui/uiTextureVS.glsl", "Ui/uiTextureFS.glsl"));
	shaders.emplace(STUiColor, new Shader("Ui/uiColorVS.glsl", "Ui/uiColorFS.glsl"));
	shaders.emplace(STSkybox, new Shader("skyboxVS.glsl", "skyboxFS.glsl"));
	shaders.emplace(STModel, new Shader("modelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STModelInstanced, new Shader("instanceModelVS.glsl", "modelFS.glsl"));
	shaders.emplace(STTexture, new Shader("textureVS.glsl", "textureFS.glsl"));
	shaders.emplace(STColor, new Shader("colorVS.glsl", "colorFS.glsl"));
	shaders.emplace(STReflect, new Shader("Ref/reflectVS.glsl", "Ref/reflectFS.glsl"));
	shaders.emplace(STRefract, new Shader("Ref/refractVS.glsl", "Ref/refractFS.glsl"));
	shaders.emplace(STDepth, new Shader("Depth/depthVS.glsl", "Depth/depthFS.glsl"));
	shaders.emplace(STDepthPoint, new GeometryShader("Depth/depthPointVS.glsl", "Depth/depthPointGS.glsl", "Depth/depthPointFS.glsl"));
	shaders.emplace(STDepthDebug, new Shader("Depth/depthDebugVS.glsl", "Depth/depthDebugFS.glsl"));
	uboLights = new UboLights(BASE_AMBIENT, 0, 0, 0, nullptr, nullptr, nullptr);
	uboTextureColor = new UboTextureColor(false, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	uboViewProjection = new UboViewProjection(glm::mat4(1.0f), glm::mat4(1.0f));
	ubos.push_back(uboLights);
	ubos.push_back(uboTextureColor);
	ubos.push_back(uboViewProjection);
	std::vector<Shader*> shaders;
	shaders.push_back(getShader(STModel));
	shaders.push_back(getShader(STModelInstanced));
	shaders.push_back(getShader(STTexture));
	shaders.push_back(getShader(STColor));

	getShader(STReflect)->bind(uboViewProjection);
	getShader(STRefract)->bind(uboViewProjection);

	for (int i = 0; i < shaders.size(); i++) {
		for (int j = 0; j < ubos.size(); j++) {
			shaders[i]->bind(ubos[j]);
		}
	}

	textRenderer = new TextRenderer(0.5f);
	textRenderer->load("res/fonts/ButterLayer.ttf", 60);

	menuScene = new MenuScene();
	goToMenu();
}

SceneManager::~SceneManager() {
	for (auto &ubo : ubos) {
		delete ubo;
	}

	for (auto &pair : shaders) {
		delete pair.second;
	}
	shaders.clear();

	if(currentScene == menuScene) {
		setCurrentScene(nullptr);
	}

	delete menuScene;
	delete currentScene;
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

GLFWwindow* SceneManager::getWindow() {
	return window;
}

void SceneManager::quit() {
	glfwSetWindowShouldClose(window, true);
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
