#include "EditorScene.h"
#include "Ui/UiText.h"
#include "Render/Camera.h"
#include "TestScene.h"
#include "Scene/Node.h"
#include "Serialization/Serializer.h"

EditorScene::EditorScene() {
	editorCamera = new Camera(glm::vec3(0.0f, 1.0f, 1.0f));
	serializer = Serializer::getInstance();
	cameraText = new UiText(glm::vec2(0.0f, windowHeight), glm::vec2(windowWidth, 80.0f), "-------------", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight, BottomLeft);
	rootUiElement->addChild(cameraText);
	setEditorCamera(useEditorCamera);
	loadTexturesModels();
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
	ImGui::Begin("Editor manager", nullptr, 64);
	if (nodeSelectionCallback != nullptr && ImGui::Button("Stop selecting graph node")) {
		nodeSelectionCallback = nullptr;
	}
	if (textureSelectionCallback != nullptr && ImGui::Button("Stop selecting texture")) {
		textureSelectionCallback = nullptr;
	}
	if (modelSelectionCallback != nullptr && ImGui::Button("Stop selecting model")) {
		modelSelectionCallback = nullptr;
	}
	if (ImGui::Button("New scene") && !showConfirmationDialog) {
		showConfirmationDialog = true;
		confirmationDialogCallback = [this]() {
			setEditedScene(new Scene());
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
	if (ImGui::Button("Reload resources") && !showConfirmationDialog) {
		confirmationDialogCallback = [this]() {
			assetManager->reloadResources();
			loadTexturesModels();
		};
		showConfirmationDialog = true;
	}
	ImGui::End();

	if (typeToCreate != SNone) {
		std::string title = "Create a ";
		switch (ttc) {
			case TTCComponent:
				title += "component";
				break;
			case TTCGraphNode:
				title += "graph node";
				break;
			case TTCMesh:
				title += "mesh";
				break;
			case TTCSkybox:
				title += "skybox";
				break;
		}
		ImGui::Begin(title.c_str(), nullptr, 64);
		switch (typeToCreate) {
			case SSkybox:
				static std::string faces[6];
				if (typeCreationStarted) {
					for (int i = 0; i < 6; i++) {
						faces[i] = "";
					}
				}
				static bool correct;
				correct = true;
				for (int i = 0; i < 6; i++) {
					if (faces[i].length() == 0) {
						correct = false;
					}
					ImGui::PushID(i);
					std::string title;
					switch (i) {
						case 0:
							title = "Right: ";
							break;
						case 1:
							title = "Left: ";
							break;
						case 2:
							title = "Top: ";
							break;
						case 3:
							title = "Bottom: ";
							break;
						case 4:
							title = "Front: ";
							break;
						case 5:
							title = "Back: ";
							break;
					}
					title += faces[i].length() == 0 ? "-" : faces[i];
					ImGui::Text((title).c_str());
					if (textureSelectionCallback == nullptr) {
						ImGui::SameLine();
						if (ImGui::Button("Change...")) {
							textureSelectionCallback = [&, i](Texture t) {
								faces[i] = t.path;
							};
						}
					}
					ImGui::PopID();
				}
				if (correct && ImGui::Button("Create")) {
					std::vector<std::string> fcs;
					for (int i = 0; i < 6; i++) {
						fcs.push_back(faces[i]);
					}
					creationCallback(new Skybox(fcs));
					setCreationTarget(SNone);
				}
				break;
		}
		typeCreationStarted = false;
		ImGui::End();
	}

	if (textureSelectionCallback != nullptr) {
		//ImGui::SetNextWindowSize(ImVec2(250.0f, 600.0f));
		ImGui::Begin("SELECT TEXTURE", nullptr, 64);
		for (auto &texture : textures) {
			ImGui::PushID(&texture);
			if (ImGui::Button(texture.c_str())) {
				textureSelectionCallback(assetManager->getTexture(texture));
				textureSelectionCallback = nullptr;
			}
			ImGui::PopID();
		}
		ImGui::End();
	}

	if (modelSelectionCallback != nullptr) {
		//ImGui::SetNextWindowSize(ImVec2(250.0f, 600.0f));
		ImGui::Begin("SELECT MODEL", nullptr, 64);
		for (auto &model : models) {
			ImGui::PushID(&model);
			if (ImGui::Button(model.c_str())) {
				modelSelectionCallback(assetManager->getModelData(model));
				modelSelectionCallback = nullptr;
			}
			ImGui::PopID();
		}
		ImGui::End();
	}

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
		static const auto BUFFER_SIZE = 50;
		static char nameBuffer[BUFFER_SIZE] = "";
		ImGui::InputText("Scene name", nameBuffer, sizeof(nameBuffer));
		if (ImGui::Button("SAVE")) {
			std::string targetName(nameBuffer);
			if (targetName.length() > 0) {
				if (useEditorCamera) {
					editedScene->setCamera(playerCamera);
				}
				serializer->saveScene(editedScene, targetName);
				if (useEditorCamera) {
					editedScene->setCamera(editorCamera);
				}
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
		ImGui::Begin("Scene objects", nullptr, 64);
		ImGui::TreePush();
		if (editedScene->getSkybox() == nullptr) {
			if (ImGui::Button("Create a skybox")) {
				setCreationTarget(SSkybox, [editedScene = editedScene](void *skybox) {
					editedScene->setSkybox(reinterpret_cast<Skybox*>(skybox));
				});
			}
		} else {
			if (ImGui::Button("Delete the skybox")) {
				Skybox *skybox = editedScene->getSkybox();
				editedScene->setSkybox(nullptr);
				delete skybox;
			}
		}
		ImGui::NewLine();
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
	if (getCursorLocked()) {
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
}

bool EditorScene::doesAnyChildContain(GraphNode* node, GraphNode* target) {
	for (auto &child : target->getChildren()) {
		if (doesAnyChildContain(node, child)) {
			return true;
		}
	}
	return false;
}

void EditorScene::setCreationTarget(SerializableType type, std::function<void(void*)> creationCallback) {
	if (type == SNone) {
		typeToCreate = SNone;
		ttc = TTCNone;
		this->creationCallback = nullptr;
		typeCreationStarted = false;
		return;
	}
	ttc = TTCNone;
	if (type == SGraphNode) {
		ttc = TTCGraphNode;
	} else if (type == SSkybox) {
		ttc = TTCSkybox;
	}
	for (int i = 0; i < sizeof(creatableComponents) / sizeof(*creatableComponents); i++) {
		if (creatableComponents[i] == type) {
			ttc = TTCComponent;
			break;
		}
	}
	for (int i = 0; i < sizeof(creatableMeshes) / sizeof(*creatableMeshes); i++) {
		if (creatableMeshes[i] == type) {
			ttc = TTCMesh;
		}
	}
	if (ttc != TTCNone) {
		typeCreationStarted = true;
		typeToCreate = type;
		this->creationCallback = creationCallback;
	}
}

void EditorScene::loadTexturesModels() {
	textures = assetManager->getTextures();
	models = assetManager->getModels();
}

void EditorScene::setEditedScene(Scene* scene, bool deletePrevious) {
	if (scene == editedScene) {
		return;
	}
	Scene *previous = editedScene;

	editedNodes.clear();
	showConfirmationDialog = false;
	showSaveDialog = false;
	showLoadDialog = false;
	confirmationDialogCallback = nullptr;
	nodeSelectionCallback = nullptr;
	setCreationTarget(SNone);

	if (scene != nullptr) {
		delete editorCamera;
		editorCamera = new Camera(glm::vec3(0.0f, 1.0f, 1.0f));
		Camera *camera = scene->getCamera();
		if (camera != nullptr) {
			delete playerCamera;
			playerCamera = camera;
		} else {
			playerCamera = new Camera(glm::vec3(0.0f, 1.0f, 1.0f));
		}
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
	if (nodeSelectionCallback != nullptr) {
		if (ImGui::Button("CHOOSE")) {
			nodeSelectionCallback(node);
			nodeSelectionCallback = nullptr;
		}
		ImGui::SameLine();
	}
	if (!opened && ImGui::Button("Open...")) {
		editedNodes.push_back(node);
	}
	ImGui::SameLine();
	if (node != editedScene->getRootNode() && nodeSelectionCallback == nullptr) {
		if (ImGui::Button("Set parent...")) {
			nodeSelectionCallback = [this, node](GraphNode *parent) {
				if (parent != nullptr && parent != node && !doesAnyChildContain(parent, node)) {
					node->setParent(parent);
				}
			};
		}
		ImGui::SameLine();
	}
	if (!node->getChildren().empty()) {
		if (ImGui::TreeNode("Children")) {
			ImGui::NewLine();
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
	if (pressed) {
		switch (key) {
			case KEY_TOGGLE_MOUSE_LOCK:
				setCursorLocked(!getCursorLocked());
				break;
			case KEY_QUIT:
				if (!showConfirmationDialog) {
					showConfirmationDialog = true;
					confirmationDialogCallback = [this]() {
						setEditedScene(nullptr);
						gameManager->goToMenu(false);
					};
				}
				break;
		}
		if (!getCursorLocked()) {
			switch (key) {
				case EDITOR_KEY_TOGGLE_CAMERA:
					setEditorCamera(!useEditorCamera);
					break;
			}
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
