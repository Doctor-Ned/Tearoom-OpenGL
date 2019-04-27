#include "EditorScene.h"
#include "Ui/UiText.h"
#include "Render/Camera.h"
#include "TestScene.h"
#include "Scene/Node.h"
#include "Serialization/Serializer.h"
#include "Mesh/Model.h"
#include "Mesh/MeshRefBox.h"
#include "Mesh/MeshBox.h"
#include "Mesh/MeshSphere.h"
#include "Mesh/MeshRefSphere.h"
#include "Mesh/MeshColorSphere.h"
#include "Mesh/MeshTorus.h"
#include "Mesh/MeshColorTorus.h"
#include "Mesh/MeshColorPlane.h"
#include "Mesh/MeshPlane.h"
#include "Mesh/MeshColorCylinder.h"
#include "Mesh/MeshCylinder.h"
#include "Mesh/MeshColorCone.h"
#include "Mesh/MeshCone.h"

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
	if (meshSelectionCallback != nullptr && ImGui::Button("Stop selecting mesh")) {
		meshSelectionCallback = nullptr;
	}
	if (nodeSelectionCallback != nullptr && ImGui::Button("Stop selecting graph node")) {
		nodeSelectionCallback = nullptr;
	}
	if (textureSelectionCallback != nullptr && ImGui::Button("Stop selecting texture")) {
		textureSelectionCallback = nullptr;
	}
	if (modelSelectionCallback != nullptr && ImGui::Button("Stop selecting model")) {
		modelSelectionCallback = nullptr;
	}
	if (shaderTypeSelectionCallback != nullptr && ImGui::Button("Stop selecting shader type")) {
		shaderTypeSelectionCallback = nullptr;
	}

	ImGui::Checkbox("Update edited scene", &updateEditedScene);
	if(updateEditedScene) {
		ImGui::SliderFloat("Time scale", &timeScale, -1.0f, 1.0f);
		ImGui::InputFloat("Time scale (fixed)", &timeScale);
	}

	if (confirmationDialogCallback == nullptr && ImGui::Button("New scene")) {
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
	if (confirmationDialogCallback == nullptr && editedScene != nullptr) {
		if (ImGui::Button("Close scene")) {
			confirmationDialogCallback = [this]() {
				setEditedScene(nullptr);
			};
		}
	}
	if (confirmationDialogCallback == nullptr && ImGui::Button("Reload resources")) {
		confirmationDialogCallback = [this]() {
			assetManager->reloadResources();
			loadTexturesModels();
		};
	}
	ImGui::End();
	static std::vector<TypeCreation*> typeCreationsToDelete;
	for (auto &typeCreation : typeCreations) {
		ImGui::PushID(typeCreation);
		std::string title = "Create a ";
		switch (typeCreation->ttc) {
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
		switch (typeCreation->typeToCreate) {
			case SGraphNode:
			{
				static const auto BUFFER_SIZE = 50;
				static char nameBuffer[BUFFER_SIZE];
				static GraphNode *node;
				if (typeCreation->typeCreationStarted) {
					nameBuffer[0] = '\0';
					node = new GraphNode();
				}
				node->drawGui();
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(node);
					node = nullptr;
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SSkybox:
			{
				static std::string faces[6];
				if (typeCreation->typeCreationStarted) {
					for (auto& face : faces) {
						face = "";
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
					typeCreation->creationCallback(new Skybox(fcs));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SModel:
			{
				static std::string model;
				if (typeCreation->typeCreationStarted) {
					model = "";
				}
				std::string title = "Model: ";
				if (model.length() == 0) {
					title += "-";
				} else {
					title += model;
				}
				ImGui::Text(title.c_str());
				if (modelSelectionCallback == nullptr) {
					ImGui::SameLine();
					if (ImGui::Button("Change...")) {
						modelSelectionCallback = [&](std::string mdl) {
							model = mdl;
						};
					}
				}
				if (model.length() > 0 && ImGui::Button("Create")) {
					Model *mdl = new Model(model);
					typeCreation->creationCallback(mdl);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshBox:
			case SMeshColorBox:
			case SMeshRefBox:
			{
				static bool useDimensions = true;
				ImGui::Checkbox("Use dimensions", &useDimensions);
				static glm::vec3 min, max, dimensions;
				static bool reflective;
				static glm::vec4 color;
				static std::string texture;
				if (typeCreation->typeCreationStarted) {
					reflective = true;
					texture = "";
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					min = glm::vec3(-0.5f, -0.5f, -0.5f);
					max = glm::vec3(0.5f, 0.5f, 0.5f);
					dimensions = glm::vec3(1.0f, 1.0f, 1.0f);
				}
				if (useDimensions) {
					ImGui::SliderFloat3("Dimensions", reinterpret_cast<float*>(&dimensions), 0.0f, 10.0f);
					ImGui::InputFloat3("Dimensions (fixed)", reinterpret_cast<float*>(&dimensions));
					min = -dimensions / 2.0f;
					max = dimensions / 2.0f;
				} else {
					ImGui::SliderFloat3("Min", reinterpret_cast<float*>(&min), -5.0f, 0.0f);
					ImGui::InputFloat3("Min (fixed)", reinterpret_cast<float*>(&min));
					ImGui::SliderFloat3("Max", reinterpret_cast<float*>(&max), 0.0f, 5.0f);
					ImGui::InputFloat3("Max (fixed)", reinterpret_cast<float*>(&max));
				}
				bool valid = true;
				for (int i = 0; i < 3; i++) {
					if (min[i] > max[i]) {
						valid = false;
						break;
					}
				}

				switch (typeCreation->typeToCreate) {
					case SMeshBox:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
						break;
					case SMeshColorBox:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
					case SMeshRefBox:
						ImGui::Checkbox("Reflective (refractive otherwise)", &reflective);
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *box;
					if(typeCreation->typeToCreate == SMeshColorBox) {
						box = new MeshColorBox(min, max, color);
					} else if(typeCreation->typeToCreate == SMeshRefBox) {
						box = new MeshRefBox(reflective, min, max);
					} else {
						//SMeshBox
						box = new MeshBox(min, max, texture);
					}
					typeCreation->creationCallback(box);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshCone:
			case SMeshColorCone:
			{
				static glm::vec4 color;
				static std::string texture;
				static float radius, height;
				static int sideAmount;
				if (typeCreation->typeCreationStarted) {
					texture = "";
					radius = 0.5f;
					height = 1.0f;
					sideAmount = 25;
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				}
				bool valid = true;

				ImGui::SliderFloat("Radius", &radius, 0.0f, 5.0f);
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::SliderFloat("Height", &height, 0.0f, 5.0f);
				ImGui::InputFloat("Height (fixed)", &height);
				ImGui::SliderInt("Side amount", &sideAmount, 3, 75);

				switch (typeCreation->typeToCreate) {
					case SMeshCone:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
					break;
					case SMeshColorCone:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *cone;
					if (typeCreation->typeToCreate == SMeshColorCone) {
						cone = new MeshColorCone(radius, height, sideAmount, color);
					} else {
						cone = new MeshCone(radius, height, sideAmount, texture);
					}
					typeCreation->creationCallback(cone);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshCylinder:
			case SMeshColorCylinder:
			{
				static glm::vec4 color;
				static std::string texture;
				static float radius, height;
				static int sideAmount;
				if (typeCreation->typeCreationStarted) {
					texture = "";
					radius = 0.5f;
					height = 1.0f;
					sideAmount = 25;
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				}
				bool valid = true;

				ImGui::SliderFloat("Radius", &radius, 0.0f, 5.0f);
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::SliderFloat("Height", &height, 0.0f, 5.0f);
				ImGui::InputFloat("Height (fixed)", &height);
				ImGui::SliderInt("Side amount", &sideAmount, 3, 75);

				switch (typeCreation->typeToCreate) {
					case SMeshCylinder:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
					break;
					case SMeshColorCylinder:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *cylinder;
					if (typeCreation->typeToCreate == SMeshColorCylinder) {
						cylinder = new MeshColorCylinder(radius, height, sideAmount, color);
					} else {
						cylinder = new MeshCylinder(radius, height, sideAmount, texture);
					}
					typeCreation->creationCallback(cylinder);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshPlane:
			case SMeshColorPlane:
			{
				static glm::vec4 color;
				static std::string texture;
				static float width, length;
				if (typeCreation->typeCreationStarted) {
					texture = "";
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					width = 1.0f;
					length = 1.0f;
				}
				bool valid = true;

				ImGui::SliderFloat("Width", &width, 0.0f, 5.0f);
				ImGui::InputFloat("Width (fixed)", &width);
				ImGui::SliderFloat("Length", &length, 0.0f, 5.0f);
				ImGui::InputFloat("Length (fixed)", &length);

				switch (typeCreation->typeToCreate) {
					case SMeshPlane:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
					break;
					case SMeshColorPlane:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *plane;
					if (typeCreation->typeToCreate == SMeshColorPlane) {
						plane = new MeshColorPlane(width, length, color);
					} else {
						plane = new MeshPlane(width, length, texture);
					}
					typeCreation->creationCallback(plane);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshSphere:
			case SMeshColorSphere:
			case SMeshRefSphere:
			{
				static bool reflective;
				static glm::vec4 color;
				static std::string texture;
				static int precision;
				static float radius;
				if (typeCreation->typeCreationStarted) {
					reflective = true;
					texture = "";
					precision = 25;
					radius = 0.5f;
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				}
				bool valid = true;

				ImGui::SliderFloat("Radius", &radius, 0.0f, 5.0f);
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::SliderInt("Precision", &precision, 3, 75);

				switch (typeCreation->typeToCreate) {
					case SMeshSphere:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
					break;
					case SMeshColorSphere:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
					case SMeshRefSphere:
						ImGui::Checkbox("Reflective (refractive otherwise)", &reflective);
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *sphere;
					if (typeCreation->typeToCreate == SMeshColorSphere) {
						sphere = new MeshColorSphere(radius, precision, color);
					} else if (typeCreation->typeToCreate == SMeshRefSphere) {
						sphere = new MeshRefSphere(reflective, radius, precision);
					} else {
						sphere = new MeshSphere(radius, precision, texture);
					}
					typeCreation->creationCallback(sphere);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SMeshTorus:
			case SMeshColorTorus:
			{
				static glm::vec4 color;
				static std::string texture;
				static float radiusIn, radiusOut;
				static int sideAmount;
				if (typeCreation->typeCreationStarted) {
					texture = "";
					radiusIn = 0.5f;
					radiusOut = 1.0f;
					sideAmount = 25;
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				}
				bool valid = true;

				ImGui::SliderFloat("Radius 1", &radiusIn, 0.0f, 5.0f);
				ImGui::InputFloat("Radius 1 (fixed)", &radiusIn);
				ImGui::SliderFloat("Radius 2", &radiusOut, 0.0f, 5.0f);
				ImGui::InputFloat("Radius 2", &radiusOut, 0.0f, 5.0f);
				ImGui::SliderInt("Side amount", &sideAmount, 3, 75);

				switch (typeCreation->typeToCreate) {
					case SMeshTorus:
					{
						showTextureGui(texture);
						if (valid && texture.length() == 0) {
							valid = false;
						}
					}
					break;
					case SMeshColorTorus:
						ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
						break;
				}

				if (valid && ImGui::Button("Create")) {
					Mesh *torus;
					if (typeCreation->typeToCreate == SMeshColorTorus) {
						torus = new MeshColorTorus(radiusIn, radiusOut, sideAmount, color);
					} else {
						torus = new MeshTorus(radiusIn, radiusOut, sideAmount, texture);
					}
					typeCreation->creationCallback(torus);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
		}
		typeCreation->typeCreationStarted = false;
		if (ImGui::Button("CANCEL")) {
			typeCreationsToDelete.push_back(typeCreation);
		}
		ImGui::End();
		ImGui::PopID();
	}

	for (auto &tc : typeCreationsToDelete) {
		deleteTypeCreation(tc);
	}

	if (meshSelectionCallback != nullptr) {
		ImGui::Begin("SELECT MESH TYPE", nullptr, 64);
		for (auto &type : creatableMeshes) {
			if (ImGui::Button(SerializableTypeNames[type].c_str())) {
				meshSelectionCallback(type);
				meshSelectionCallback = nullptr;
			}
		}
		if (ImGui::Button("CANCEL")) {
			meshSelectionCallback = nullptr;
		}
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
		if (ImGui::Button("CANCEL")) {
			textureSelectionCallback = nullptr;
		}
		ImGui::End();
	}

	if (modelSelectionCallback != nullptr) {
		//ImGui::SetNextWindowSize(ImVec2(250.0f, 600.0f));
		ImGui::Begin("SELECT MODEL", nullptr, 64);
		for (auto &model : models) {
			ImGui::PushID(&model);
			if (ImGui::Button(model.c_str())) {
				modelSelectionCallback(model);
				//modelSelectionCallback(assetManager->getModelData(model));
				modelSelectionCallback = nullptr;
			}
			ImGui::PopID();
		}
		if (ImGui::Button("CANCEL")) {
			modelSelectionCallback = nullptr;
		}
		ImGui::End();
	}

	if (shaderTypeSelectionCallback != nullptr) {
		ImGui::Begin("SELECT SHADER TYPE", nullptr, 64);
		for (int i = 0; i < sizeof(ShaderTypeNames) / sizeof(*ShaderTypeNames); i++) {
			std::string shaderType = ShaderTypeNames[i];
			ImGui::PushID(shaderType.c_str());
			if (ImGui::Button(shaderType.c_str())) {
				shaderTypeSelectionCallback(ShaderTypes[i]);
				shaderTypeSelectionCallback = nullptr;
			}
			ImGui::PopID();
		}
		if (ImGui::Button("CANCEL")) {
			shaderTypeSelectionCallback = nullptr;
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
				if (confirmationDialogCallback == nullptr && ImGui::Button(name.c_str())) {
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
		std::string targetName(nameBuffer);
		targetName = Global::trim(targetName);
		if (targetName.length() > 0) {
			if (ImGui::Button("SAVE")) {
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
	if (confirmationDialogCallback != nullptr) {
		ImGui::Begin("Confirmation dialog", nullptr, 64);
		ImGui::Text("Are you sure?");
		if (ImGui::Button("Yes")) {
			confirmationDialogCallback();
			confirmationDialogCallback = nullptr;
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) {
			confirmationDialogCallback = nullptr;
		}
		ImGui::End();
	}
	if (editedScene != nullptr) {
		ImGui::Begin("Scene objects", nullptr, 64);
		ImGui::TreePush();
		if (editedScene->getSkybox() == nullptr) {
			if (!typeCreationExists(SSkybox) && ImGui::Button("Create a skybox")) {
				addTypeCreation(SSkybox, [editedScene = editedScene](void *skybox) {
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
	if (editedScene != nullptr && updateEditedScene) {
		editedScene->update(deltaTime * timeScale);
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

void EditorScene::deleteTypeCreation(TypeCreation* typeCreation) {
	for (auto i = typeCreations.begin(); i != typeCreations.end();) {
		if (*i == typeCreation) {
			typeCreations.erase(i);
			delete typeCreation;
			return;
		}
		++i;
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

void EditorScene::addEditedNode(GraphNode* node) {
	for (auto &nod : editedNodes) {
		if (nod == node) {
			return;
		}
	}
	editedNodes.push_back(node);
}

void EditorScene::showTextureGui(std::string& texture) {
	std::string txt = "Texture: ";
	if (texture.length() == 0) {
		txt += "-";
	} else {
		txt += texture;
	}
	ImGui::Text(txt.c_str());
	if (textureSelectionCallback == nullptr) {
		ImGui::SameLine();
		if (ImGui::Button("Change...")) {
			textureSelectionCallback = [&](Texture t) {
				texture = t.path;
			};
		}
	}
}

void EditorScene::addTypeCreation(SerializableType type, std::function<void(void*)> creationCallback) {
	if (type == SNone || typeCreationExists(type)) {
		return;
	}
	TypeCreation *creation = new TypeCreation();
	creation->ttc = TTCNone;
	if (type == SGraphNode) {
		creation->ttc = TTCGraphNode;
	} else if (type == SSkybox) {
		creation->ttc = TTCSkybox;
	}
	for (int i = 0; i < sizeof(creatableComponents) / sizeof(*creatableComponents); i++) {
		if (creatableComponents[i] == type) {
			creation->ttc = TTCComponent;
			break;
		}
	}
	for (int i = 0; i < sizeof(creatableMeshes) / sizeof(*creatableMeshes); i++) {
		if (creatableMeshes[i] == type) {
			creation->ttc = TTCMesh;
		}
	}
	if (creation->ttc != TTCNone) {
		creation->typeCreationStarted = true;
		creation->typeToCreate = type;
		creation->creationCallback = creationCallback;
		typeCreations.push_back(creation);
	} else {
		delete creation;
	}
}

bool EditorScene::typeCreationExists(SerializableType type) {
	for (auto &tc : typeCreations) {
		switch (type) {
			default:
				if (tc->typeToCreate == type) {
					return true;
				}
				break;
			case SMeshBox:
			case SMeshColorBox:
			case SMeshRefBox:
				switch (tc->typeToCreate) {
					case SMeshBox:
					case SMeshColorBox:
					case SMeshRefBox:
						return true;
				}
				break;
			case SMeshCone:
			case SMeshColorCone:
				switch (tc->typeToCreate) {
					case SMeshCone:
					case SMeshColorCone:
						return true;
				}
				break;
			case SMeshCylinder:
			case SMeshColorCylinder:
				switch (tc->typeToCreate) {
					case SMeshCylinder:
					case SMeshColorCylinder:
						return true;
				}
				break;
			case SMeshPlane:
			case SMeshColorPlane:
				switch (tc->typeToCreate) {
					case SMeshPlane:
					case SMeshColorPlane:
						return true;
				}
				break;
			case SMeshSphere:
			case SMeshColorSphere:
			case SMeshRefSphere:
				switch (tc->typeToCreate) {
					case SMeshSphere:
					case SMeshColorSphere:
					case SMeshRefSphere:
						return true;
				}
				break;
			case SMeshTorus:
			case SMeshColorTorus:
				switch (tc->typeToCreate) {
					case SMeshTorus:
					case SMeshColorTorus:
						return true;
				}
				break;
		}
	}
	return false;
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
	showSaveDialog = false;
	showLoadDialog = false;
	confirmationDialogCallback = nullptr;
	nodeSelectionCallback = nullptr;
	for (auto &tc : typeCreations) {
		delete tc;
	}
	typeCreations.clear();

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
	if (!typeCreationExists(SGraphNode) && ImGui::Button("Add child...")) {
		addTypeCreation(SGraphNode, [this, node](void* nod) {
			if (nod != nullptr) {
				appendNode(reinterpret_cast<GraphNode*>(nod), node);
			}
		});
	}
	ImGui::SameLine();
	if (node != editedScene->getRootNode()) {
		if (nodeSelectionCallback == nullptr) {
			if (ImGui::Button("Set parent...")) {
				nodeSelectionCallback = [this, node](GraphNode *parent) {
					if (parent != nullptr && parent != node && !doesAnyChildContain(parent, node)) {
						node->setParent(parent);
					}
				};
			}
			ImGui::SameLine();
		}
		if (this->confirmationDialogCallback == nullptr && ImGui::Button("Delete")) {
			confirmationDialogCallback = [this, node]() {
				editedScene->removeNode(node);
			};
		}
		ImGui::SameLine();
	}
	if (!node->getChildren().empty()) {
		if (ImGui::TreeNode("Children")) {
			//ImGui::NewLine();
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
				if (confirmationDialogCallback == nullptr) {
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
