#include "EditorScene.h"
#include "Ui/UiText.h"
#include "Render/Camera.h"
#include "TestScene.h"
#include "Scene/Node.h"
#include "Serialization/Serializer.h"

EditorScene::EditorScene() {
	editorCamera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
	playerCamera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
	serializer = Serializer::getInstance();
	cameraText = new UiText(glm::vec2(0.0f, windowHeight), glm::vec2(windowWidth, 80.0f), "-------------", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight, BottomLeft);
	rootUiElement->addChild(cameraText);
	setEditorCamera(useEditorCamera);
	reinitializeRenderMap();
}

void EditorScene::render() {
	if (editedScene != nullptr) {
		editedScene->render();
	}
}

void EditorScene::renderUi() {
	if (editedScene != nullptr) {
		editedScene->renderUi();
	}
	Scene::renderUi();
	idCounter = 0;
	ImGui::Begin("Scene manager", nullptr, 64);
	if (ImGui::Button("New scene") && !showConfirmationDialog) {
		showConfirmationDialog = true;
		confirmationDialogCallback = [this]() {
			setEditedScene(new Scene());
			std::vector<std::string> faces;
			faces.emplace_back("res/skybox/test/right.jpg");
			faces.emplace_back("res/skybox/test/left.jpg");
			faces.emplace_back("res/skybox/test/top.jpg");
			faces.emplace_back("res/skybox/test/bottom.jpg");
			faces.emplace_back("res/skybox/test/front.jpg");
			faces.emplace_back("res/skybox/test/back.jpg");
			editedScene->setSkybox(new Skybox(assetManager->getShader(STSkybox), faces));
		};
	}
	if (!showLoadDialog) {
		if (ImGui::Button("Load scene...")) {
			showLoadDialog = true;
		}
	}
	if (editedScene != nullptr && !showSaveDialog) {
		if (ImGui::Button("Save scene...")) {
			showSaveDialog = true;
		}
	}
	if (editedScene != nullptr && !showConfirmationDialog) {
		if (ImGui::Button("Close scene")) {
			confirmationDialogCallback = [this]() {
				setEditedScene(nullptr);
			};
			showConfirmationDialog = true;
		}
	}
	ImGui::End();

	if (showLoadDialog) {
		// flag reference: https://pyimgui.readthedocs.io/en/latest/reference/imgui.html
		ImGui::Begin("Load a scene from file", nullptr, 64);
		std::vector<std::string> scenes = serializer->getSceneNames();
		if (scenes.empty()) {
			ImGui::Text("No saved scene files detected.");
		} else {
			ImGui::Text(("Found " + std::to_string(scenes.size()) + " available scenes:").c_str());
			ImGui::Indent();
			for (auto &name : scenes) {
				if (ImGui::Button(name.c_str()) && !showConfirmationDialog) {
					showConfirmationDialog = true;
					confirmationDialogCallback = [this, name]() {
						setEditedScene(serializer->loadScene(name));
					};
					showLoadDialog = false;
				}
			}
			ImGui::Unindent();
		}
		if (ImGui::Button("CLOSE")) {
			showLoadDialog = false;
		}
		ImGui::End();
	}

	if (showSaveDialog && editedScene != nullptr) {
		ImGui::Begin("Enter scene name to save it", nullptr, 64);
		static const int BUFFER_SIZE = 50;
		static char nameBuffer[BUFFER_SIZE];
		for (int i = 0; i < BUFFER_SIZE; i++) {
			nameBuffer[i] = '\0';
		}
		ImGui::InputText("Scene name: ", nameBuffer, IM_ARRAYSIZE(nameBuffer));
		if (ImGui::Button("SAVE")) {
			std::string targetName(nameBuffer);
			if (targetName.length() > 0) {
				serializer->saveScene(editedScene, targetName);
				showSaveDialog = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL")) {
			showSaveDialog = false;
		}
		ImGui::End();
	}
	if (showConfirmationDialog) {
		if (confirmationDialogCallback == nullptr) {
			showConfirmationDialog = false;
		} else {
			ImGui::Begin("Confirmation dialog", nullptr, 64);
			ImGui::Text("Are you sure?");
			if (ImGui::Button("Yes")) {
				if (confirmationDialogCallback != nullptr) {
					confirmationDialogCallback();
					confirmationDialogCallback = nullptr;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				confirmationDialogCallback = nullptr;
				showConfirmationDialog = false;
			}
			ImGui::End();
		}
	}
	if (editedScene != nullptr) {
		ImGui::Begin("Scene graph", nullptr, 64);
		ImGui::TreePush();
		showNodeAsTree(editedScene->rootNode);
		ImGui::TreePop();
		ImGui::End();

		ImGui::Begin("Object builder", nullptr, 64);
		if (ImGui::Button("Add box")) {
			appendNode(Node::createBox(glm::vec3(1), Node::getRandomColor()));
		}
		ImGui::End();
		static std::vector<GraphNode*> toDelete;
		for (auto &node : editedNodes) {
			ImGui::PushID(idCounter++);
			ImGui::Begin(("Node '" + node->getName() + "'").c_str());
			if (ImGui::Button("Close")) {
				toDelete.push_back(node);
			}
			node->drawGui();
			ImGui::End();
			ImGui::PopID();
		}
		if (!toDelete.empty()) {
			editedNodes.erase(std::remove_if(editedNodes.begin(), editedNodes.end(), [](const auto&x) {
				return std::find(toDelete.begin(), toDelete.end(), x) != toDelete.end();
			}), editedNodes.end());
			toDelete.clear();
		}
	}
}

Camera* EditorScene::getCamera() {
	return useEditorCamera ? editorCamera : playerCamera;
}

void EditorScene::update(double deltaTime) {
	if (editedScene != nullptr) {
		editedScene->update(deltaTime);
	}
	Scene::update(deltaTime);
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
}

void EditorScene::setEditedScene(Scene* scene, bool deletePrevious) {
	Scene *previous = editedScene;
	editedNodes.clear();
	if (scene != nullptr) {
		scene->setCamera(useEditorCamera ? editorCamera : playerCamera);
		lightManager->replaceLights(scene->getLights());
	} else {
		lightManager->clearLights();
	}
	editedScene = scene;
	if (deletePrevious && previous != nullptr) {
		delete previous;
	}
}

void EditorScene::appendNode(GraphNode* node, GraphNode* parent) {
	if (parent == nullptr) {
		parent = editedScene->getRootNode();
	}
	if (node->getParent() == nullptr) {
		node->setParent(parent);
	}
	if (node->getName() == "Node") {
		node->setName("Node #" + std::to_string(nodeCounter++));
	}
	editedScene->addRenderedNode(node, node->getParent());
}

void EditorScene::showNodeAsTree(GraphNode* node) {
	ImGui::PushID(idCounter++);
	ImGui::Text(node->getName().c_str());
	ImGui::SameLine();
	bool opened = false;
	for (auto i = editedNodes.begin(); i != editedNodes.end();) {
		if (*i == node) {
			opened = true;
			break;
		}
		++i;
	}
	if (!opened && ImGui::Button("Open...")) {
		editedNodes.push_back(node);
	}
	if (!node->getChildren().empty()) {
		if (ImGui::TreeNode("Children")) {
			for (auto child : node->getChildren()) {
				showNodeAsTree(child);
			}
			ImGui::TreePop();
		}
	} else {
		//ImGui::Text("No children");
	}
	ImGui::NewLine();
	ImGui::PopID();
}

void EditorScene::keyEvent(int key, bool pressed) {
	if (pressed && !showSaveDialog) {
		switch (key) {
			case EDITOR_KEY_TOGGLE_CAMERA:
				setEditorCamera(!useEditorCamera);
				break;
			case KEY_TOGGLE_MOUSE_LOCK:
				setCursorLocked(!getCursorLocked());
				break;
			case KEY_QUIT:
				if (gameManager->getKeyState(GLFW_KEY_LEFT_SHIFT) && !showConfirmationDialog) {
					showConfirmationDialog = true;
					confirmationDialogCallback = [this]() {
						gameManager->goToMenu();
					};
				}
				break;
		}
	}
}

void EditorScene::setEditorCamera(bool enabled) {
	useEditorCamera = enabled;
	if (editedScene != nullptr) {
		editedScene->setCamera(useEditorCamera ? editorCamera : playerCamera);
	}
	cameraText->setText(enabled ? "Editor camera" : "Player camera");
}
