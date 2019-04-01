#include "Scene.h"
#include "SceneManager.h"

void Scene::render() {
	rootNode->draw();
	for(auto &elem : uiElements) {
		elem->render();
	}
}

void Scene::update(double deltaTime) {
	rootNode->update(deltaTime);
}

void Scene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
	for (auto &elem : uiElements) {
		elem->updateProjection(windowWidth, windowHeight, screenWidth, screenHeight);
	}
}

void Scene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	for(auto &elem : uiElements) {
		elem->mouse_callback(window, xpos, ypos);
	}
}

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	for(auto &elem : uiElements) {
		elem->mouse_button_callback(window, butt, action, mods);
	}
}

Scene::Scene() {
	sceneManager = SceneManager::getInstance();
	windowWidth = sceneManager->getWindowWidth();
	windowHeight = sceneManager->getWindowHeight();
	screenWidth = sceneManager->getScreenWidth();
	screenHeight = sceneManager->getScreenHeight();
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
}

Scene::~Scene() {
	for (auto &elem : uiElements) {
		delete elem;
	}
	uiElements.clear();
	delete rootNode;
}
