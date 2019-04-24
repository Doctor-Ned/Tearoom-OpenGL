#include "EditorScene.h"
#include "Ui/UiText.h"
#include "Render/Camera.h"
#include "TestScene.h"
#include "Scene/Node.h"

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
	ImGui::Begin("Object builder");
	if(ImGui::Button("Add box")) {
		appendNode(Node::createBox(glm::vec3(1), Node::getRandomColor()));
	}
	ImGui::End();
	ImGui::Begin("Scene graph");
	ImGui::TreePush();
	showNodeAsTree(rootNode);
	ImGui::TreePop();
	ImGui::End();
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

void EditorScene::appendNode(GraphNode* node, GraphNode* parent) {
	if(parent == nullptr) {
		parent = rootNode;
	}
	if(node->getParent() == nullptr) {
		node->setParent(parent);
	}
	if(node->getName() == "Node") {
		node->setName("Node #" + std::to_string(nodeCounter++));
	}
	addRenderedNode(node, node->getParent());
}

void EditorScene::showNodeAsTree(GraphNode* node) const {
	if(ImGui::TreeNode(node->getName().c_str())) {
		ImGui::SameLine();
		if(ImGui::Button("Open")) {
			//todo
		}
		for(auto child : node->getChildren()) {
			showNodeAsTree(child);
		}
 		ImGui::TreePop();
	}
}

void EditorScene::keyEvent(int key, bool pressed) {
	if (pressed) {
		switch (key) {
			case EDITOR_KEY_TOGGLE_CAMERA:
				setEditorCamera(!useEditorCamera);
			break;
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
