#include "Scene.h"
#include "Scene/GameManager.h"
#include "Scene/AssetManager.h"
#include "Render/LightManager.h"

void Scene::render() {
	rootNode->draw();
}

void Scene::renderUi() {
	for (auto &elem : uiElements) {
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
	for (auto &elem : uiElements) {
		elem->mouse_callback(window, xpos, ypos);
	}
}

void Scene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	for (auto &elem : uiElements) {
		elem->mouse_button_callback(window, butt, action, mods);
	}
}

Scene::Scene() {
	gameManager = GameManager::getInstance();
	assetManager = AssetManager::getInstance();
	lightManager = LightManager::getInstance();
	gameFramebuffers = gameManager->getFramebuffers();
	windowWidth = gameManager->getWindowWidth();
	windowHeight = gameManager->getWindowHeight();
	screenWidth = gameManager->getScreenWidth();
	screenHeight = gameManager->getScreenHeight();
	windowCenterX = windowWidth / 2.0f;
	windowCenterY = windowHeight / 2.0f;
}

Scene::~Scene() {
	for (auto &elem : uiElements) {
		delete elem;
	}
	OctreeNode::toInsert2.clear();
	uiElements.clear();
	delete rootNode;
}
