#include "MiszukScene.h"
#include "Mesh/MeshColorPlane.h"
#include "SceneManager.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/Model.h"
#include "TestScene.h"
#include "Mesh/MeshColorBox.h"

MiszukScene::MiszukScene() {
	uboLights = sceneManager->getUboLights();
	uboTextureColor = sceneManager->getUboTextureColor();
	uboViewProjection = sceneManager->getUboViewProjection();
	MeshColorPlane *plane = new MeshColorPlane(1.0f, 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	MeshColorSphere *sphere = new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	MeshColorBox* box = new MeshColorBox(glm::vec3(-1,-1,-1), glm::vec3(1,1,1), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	Model* sphere2 = new Model("res/models/sphere/sphere.obj");
	GraphNode* planeNode = new GraphNode(plane);
	GraphNode* sphereNode = new GraphNode(sphere);
	GraphNode* boxNode = new GraphNode(box);
	GraphNode* sphereNode2 = new GraphNode(sphere2);
	sphereNode->localTransform.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	planeNode->localTransform.RotateByRadians(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	rootNode->addChild(planeNode);
	rootNode->addChild(sphereNode);
	rootNode->addChild(boxNode);
	rootNode->addChild(sphereNode2);
	camera = new Camera();
	updatableShaders.push_back(sceneManager->getShader(STModel));
	updatableShaders.push_back(sceneManager->getShader(STModelInstanced));
	updatableShaders.push_back(sceneManager->getShader(STTexture));
	updatableShaders.push_back(sceneManager->getShader(STColor));
	updatableShaders.push_back(sceneManager->getShader(STReflect));
	updatableShaders.push_back(sceneManager->getShader(STRefract));

	boxNode->localTransform.Translate(glm::vec3(2.0f, 0.0f, 0.0f));

	sphereNode2->localTransform.Translate(glm::vec3(-2.0f, 0.0f, 0.0f));
}

void MiszukScene::render() {
	for (auto &shader : updatableShaders) {
		shader->setViewPosition(camera->getPos());
	}
	uboViewProjection->inject(camera->getView(), projection);
	rootNode->draw();
	for (auto &elem : uiElements) {
		elem->render();
	}
	sceneManager->getTextRenderer()->renderText("Miszuk Scene", SceneManager::getInstance()->getScreenWidth() / 2, SceneManager::getInstance()->getScreenHeight() / 2, 1.0f);
}

void MiszukScene::update(double deltaTime) {

	if (getKeyState(KEY_FORWARD)) {
		camera->moveForward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_BACKWARD)) {
		camera->moveBackward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_LEFT)) {
		camera->moveLeft(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_RIGHT)) {
		camera->moveRight(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_UP)) {
		camera->moveUp(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_DOWN)) {
		camera->moveDown(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_MOUSE_LEFT)) {
		camera->rotateX(-movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_RIGHT)) {
		camera->rotateX(movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_UP)) {
		camera->rotateY(-movementSpeed * deltaTime);
	}
	if (getKeyState(KEY_MOUSE_DOWN)) {
		camera->rotateY(-movementSpeed * deltaTime);
	}

	if (abs(mouseMovementX) < 1000.0f) {
		camera->rotateX(mouseMovementX * deltaTime);
	}
	if (abs(mouseMovementY) < 1000.0f) {
		camera->rotateY(-mouseMovementY * deltaTime);
	}
	mouseMovementX = 0.0f;
	mouseMovementY = 0.0f;

	rootNode->getChild(3)->localTransform.Rotate(40.0f * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
	rootNode->getChild(2)->localTransform.Rotate(40.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
	rootNode->update(deltaTime);

}

void MiszukScene::keyboard_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (getKeyState(key)) {
			setKeyState(key, false);
			keyEvent(key, false);
		}
	}
	if (action == GLFW_PRESS) {
		if (!getKeyState(key)) {
			setKeyState(key, true);
			keyEvent(key, true);
		}
	}
}

void MiszukScene::mouse_callback(GLFWwindow * window, double xpos, double ypos) {
	Scene::mouse_callback(window, xpos, ypos);
	if (initMouse) {
		mouseX = xpos;
		mouseY = ypos;
		initMouse = false;
	}
	else {
		mouseMovementX += xpos - mouseX;
		mouseMovementY += ypos - mouseY;
		mouseX = xpos;
		mouseY = ypos;
	}
}

void MiszukScene::mouse_button_callback(GLFWwindow * window, int butt, int action, int mods) {
	Scene::mouse_button_callback(window, butt, action, mods);
}

void MiszukScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

bool MiszukScene::getKeyState(int key) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		return pair->second;
	}
	return false;
}

void MiszukScene::setKeyState(int key, bool pressed) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		pair->second = pressed;
	}
	else {
		keyStates.emplace(key, pressed);
	}
}

void MiszukScene::keyEvent(int key, bool pressed) {
	switch (key) {
	case KEY_FAST:
		if (pressed) {
			movementSpeed *= 2.0f;
		}
		else {
			movementSpeed /= 2.0f;
		}
		break;
	case KEY_SLOW:
		if (!pressed) {
			movementSpeed *= 2.0f;
		}
		else {
			movementSpeed /= 2.0f;
		}
		break;
	}
}
