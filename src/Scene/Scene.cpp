#include "Scene.h"
#include "SceneManager.h"

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

void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {}

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {}

Scene::Scene() {
	sceneManager = SceneManager::getInstance();
}

Scene::~Scene() {
	for (auto &elem : uiElements) {
		delete elem;
	}
	uiElements.clear();
	delete rootNode;
}
