#include "TestScene.h"
#include "Mesh/MeshColorPlane.h"
#include "SceneManager.h"
#include "Mesh/MeshColorSphere.h"

TestScene::TestScene() {
	uboLights = sceneManager->getUboLights();
	uboTextureColor = sceneManager->getUboTextureColor();
	uboViewProjection = sceneManager->getUboViewProjection();
	MeshColorPlane *plane = new MeshColorPlane(1.0f, 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	plane->setUseLight(false);
	MeshColorSphere *sphere = new MeshColorSphere(0.25f, 30, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	sphere->setUseLight(false);
	GraphNode* planeNode = new GraphNode(plane);
	GraphNode* sphereNode = new GraphNode(sphere);
	sphereNode->setLocal(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	//planeNode->setLocal(translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	planeNode->setLocal(rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
	rootNode->addChild(planeNode);
	rootNode->addChild(sphereNode);
	camera = new Camera();
	updatableShaders.push_back(sceneManager->getShader(STModel));
	updatableShaders.push_back(sceneManager->getShader(STModelInstanced));
	updatableShaders.push_back(sceneManager->getShader(STTexture));
	updatableShaders.push_back(sceneManager->getShader(STColor));
	updatableShaders.push_back(sceneManager->getShader(STReflect));
	updatableShaders.push_back(sceneManager->getShader(STRefract));
}

void TestScene::render() {
	for(auto &shader : updatableShaders) {
		shader->setViewPosition(camera->getPos());
	}
	uboViewProjection->inject(camera->getView(), projection);
	rootNode->draw();
	for (auto &elem : uiElements) {
		elem->render();
	}
	sceneManager->getTextRenderer()->renderText("dupa", 0, 0, 1.0f, false);
}

void TestScene::update(double deltaTime) {
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

	camera->rotateX(mouseMovementX * deltaTime);
	mouseMovementX = 0.0f;
	camera->rotateY(mouseMovementY * deltaTime);
	mouseMovementY = 0.0f;

	rootNode->update(deltaTime);
}

void TestScene::keyboard_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (getKeyState(key)) {
			setKeyState(key, true);
			keyEvent(key, true);
		}
	}
	if (action == GLFW_PRESS) {
		if (!getKeyState(key)) {
			setKeyState(key, true);
			keyEvent(key, true);
		}
	}
}

void TestScene::mouse_callback(GLFWwindow * window, double xpos, double ypos) {
	if (initMouse) {
		mouseX = xpos;
		mouseY = ypos;
		initMouse = false;
	} else {
		mouseMovementX += xpos - mouseX;
		mouseMovementY += ypos - mouseY;
		mouseX = xpos;
		mouseY = ypos;
	}
}

void TestScene::mouse_button_callback(GLFWwindow * window, int butt, int action, int mods) {}

void TestScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	Scene::updateWindowSize(windowWidth, windowHeight, screenWidth, screenHeight);
	projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

bool TestScene::getKeyState(int key) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		return pair->second;
	}
	return false;
}

void TestScene::setKeyState(int key, bool pressed) {
	auto pair = keyStates.find(key);
	if (pair != keyStates.end()) {
		pair->second = pressed;
	}
}

void TestScene::keyEvent(int key, bool pressed) {
	switch (key) {
		case KEY_FAST:
			if (pressed) {
				movementSpeed *= 2.0f;
			} else {
				movementSpeed /= 2.0f;
			}
			break;
		case KEY_SLOW:
			if (!pressed) {
				movementSpeed *= 2.0f;
			} else {
				movementSpeed /= 2.0f;
			}
			break;
	}
}
