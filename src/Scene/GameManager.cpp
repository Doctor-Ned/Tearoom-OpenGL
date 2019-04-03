#include "GameManager.h"
#include "GLFW/glfw3.h"
#include "AssetManager.h"

GameManager *GameManager::getInstance() {
	static GameManager* instance = nullptr;
	if (instance == nullptr) {
		instance = new GameManager();
	}
	return instance;
}

void GameManager::setCurrentScene(Scene * scene) {
	currentScene = scene;
	if (currentScene != nullptr) {
		currentScene->updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void GameManager::setCurrentSceneAndDeletePrevious(Scene* scene) {
	Scene* old = currentScene;
	currentScene = scene;

	delete old;

}

float GameManager::getWindowWidth() {
	return windowWidth;
}

float GameManager::getWindowHeight() {
	return windowHeight;
}

float GameManager::getScreenWidth() {
	return screenWidth;
}

float GameManager::getScreenHeight() {
	return screenHeight;
}

float GameManager::getWindowCenterX() {
	return windowCenterX;
}

float GameManager::getWindowCenterY() {
	return windowCenterY;
}

void GameManager::setWindow(GLFWwindow* window) {
	this->window = window;
}

void GameManager::setCursorLocked(bool locked) {
	glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GameManager::goToMenu(bool destroyPreviousScene) {
	Scene* old = currentScene;
	currentScene = menuScene;
	setCursorLocked(false);
	if (old != menuScene && destroyPreviousScene && reinterpret_cast<int>(old) != 0xCDCDCDCD) {
		delete old;
	}
}

void GameManager::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
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

void GameManager::setup() {
	AssetManager::getInstance()->setup();
	menuScene = new MenuScene();
	goToMenu();
}

GameManager::~GameManager() {
	if(currentScene == menuScene) {
		setCurrentScene(nullptr);
	}

	delete menuScene;
	delete currentScene;
}

void GameManager::render() {
	if (currentScene != nullptr) {
		currentScene->render();
	}
}

void GameManager::update(double timeDelta) {
	if (currentScene != nullptr) {
		currentScene->update(timeDelta);
	}
}

void GameManager::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (currentScene != nullptr) {
		currentScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void GameManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (currentScene != nullptr) {
		currentScene->mouse_callback(window, xpos, ypos);
	}
}

void GameManager::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	if (currentScene != nullptr) {
		currentScene->mouse_button_callback(window, butt, action, mods);
	}
}

GLFWwindow* GameManager::getWindow() {
	return window;
}

void GameManager::quit() {
	glfwSetWindowShouldClose(window, true);
}

GLuint GameManager::getFramebuffer() {
	return framebuffer;
}

void GameManager::setFramebuffer(GLuint fbo) {
	framebuffer = fbo;
}
