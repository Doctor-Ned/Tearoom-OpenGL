#include "EditorScene.h"
#include "Ui/UiText.h"
#include "Render/Camera.h"
#include "TestScene.h"

EditorScene::EditorScene() {
	editorCamera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
	playerCamera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
	cameraText = new UiText(glm::vec2(0.0f, windowHeight - 20.0f), glm::vec2(20.0f, 300.0f), "-------------", glm::vec3(1.0f, 1.0f, 1.0f), Fit, false);
	setEditorCamera(useEditorCamera);
}

void EditorScene::render() {
	Scene::render();
}

void EditorScene::renderUi() {
	Scene::renderUi();
}

Camera* EditorScene::getCamera() {
	return useEditorCamera ? editorCamera : playerCamera;
}

void EditorScene::update(double deltaTime) {
	if (getKeyState(KEY_FORWARD)) {
		getCamera()->moveForward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_BACKWARD)) {
		getCamera()->moveBackward(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_LEFT)) {
		getCamera()->moveLeft(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_RIGHT)) {
		getCamera()->moveRight(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_UP)) {
		getCamera()->moveUp(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_DOWN)) {
		getCamera()->moveDown(deltaTime * movementSpeed);
	}
	if (getKeyState(KEY_MOUSE_LEFT)) {
		getCamera()->rotateX(-movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_RIGHT)) {
		getCamera()->rotateX(movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_UP)) {
		getCamera()->rotateY(movementSpeed * deltaTime * 5.0f);
	}
	if (getKeyState(KEY_MOUSE_DOWN)) {
		getCamera()->rotateY(-movementSpeed * deltaTime * 5.0f);
	}

	rootNode->update(deltaTime);

	OctreeNode::getInstance()->RebuildTree(15.0f);
	OctreeNode::getInstance()->Calculate();
	OctreeNode::getInstance()->CollisionTests();
	getCamera()->RecalculateFrustum();
	Frustum frustum = getCamera()->getFrustum();
	OctreeNode::getInstance()->frustumCulling(frustum);
}

void EditorScene::keyEvent(int key, bool pressed) {
	if (pressed) {
		switch (key) {
			case KEY_TOGGLE_MOUSE_LOCK:
				setCursorLocked(!getCursorLocked());
				break;
			case KEY_QUIT:
				if (gameManager->getKeyState(GLFW_KEY_LEFT_SHIFT)) {
					gameManager->goToMenu();
				}
				break;
		}
	}
}

void EditorScene::setEditorCamera(bool enabled) {
	useEditorCamera = enabled;
	cameraText->setText(enabled ? "Editor camera" : "Player camera");
}
