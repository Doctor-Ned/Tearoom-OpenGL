#include "EditorScene.h"
#include "Ui/UiText.h"
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
#include "Scene/Components/Billboard.h"
#include "Scene/Components/AnimationController.h"
#include "Scene/Components/Animation.h"
#include "Scene/Components/RotatingObject.h"
#include "Scene/Components/PhysicalObject.h"
#include "Scene/Scripts/CollectableObject.h"
#include "Scene/Scripts/CollisionTest.h"
#include "Scene/Scripts/Picking.h"
#include "Scene/Scripts/PlayerMovement.h"
#include "Scene/Components/LightComponents/SpotLightComp.h"
#include "Scene/Components/LightComponents/PointLightComp.h"
#include "Scene/Components/KeyFrameAnimation.h"
#include "Mesh/AnimatedModel.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/SunController.h"
#include "Mesh/MeshText.h"
#include "Scene/Scripts/AnimTimeSaver.h"
#include "Scene/Scripts/ColumnMoving.h"
#include "Scene/Scripts/SunTimeActivator.h"
#include "Scene/Scripts/CollectableWatch.h"

EditorScene::EditorScene() {
	baseData.translation = glm::vec3(0.0f, 1.0f, 0.0f);
	baseData.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	baseData.eulerRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	GraphNode *camNode = new GraphNode();
	PlayerMovement *mov = new PlayerMovement(camNode);
	mov->setFly(true);
	mov->setGravity(false);
	camNode->addComponent(mov);
	editorCamera = camNode->getComponent<Camera>();
	camNode->localTransform.setData(baseData);
	serializer = Serializer::getInstance();
	cameraText = new UiText(glm::vec2(0.0f, UI_REF_HEIGHT), glm::vec2(UI_REF_WIDTH, 80.0f), "-------------", glm::vec3(1.0f, 1.0f, 1.0f), MatchHeight, BottomLeft);
	rootUiElement->addChild(cameraText);
	setEditorCamera(useEditorCamera);
	loadTexturesModels();
	reinitializeRenderMap();
}

void EditorScene::render() {
	editorCamera->getGameObject()->updateDrawData();
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
	if (nodeSelectionCallback != nullptr && ImGui::Button("Stop selecting node")) {
		nodeSelectionCallback = nullptr;
	}
	if (componentSelectionCallback != nullptr && ImGui::Button("Stop selecting component")) {
		componentSelectionCallback = nullptr;
	}
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
	if(prefabSelectionCallback != nullptr && ImGui::Button("Stop selecting prefab")) {
		prefabSelectionCallback = nullptr;
	}

	ImGui::Begin("Light manager", nullptr, 64);
	lightManager->drawGui();
	ImGui::End();

	ImGui::Checkbox("Update edited scene", &updateEditedScene);
	if (updateEditedScene) {
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
	if (editedScene != nullptr) {
		if (nodeSelectionCallback == nullptr && ImGui::Button("Choose scene camera")) {
			nodeSelectionCallback = [this](GraphNode *node) {
				Camera *camera = node->getComponent<Camera>();
				if (camera) {
					playerCamera = camera;
					setEditorCamera(false);
				}
			};
		}
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
				typeCreation->creationCallback(new GraphNode());
				typeCreationsToDelete.push_back(typeCreation);
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
			case SAnimatedModel:
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
					void* mdl = typeCreation->typeToCreate == SModel ? static_cast<void*>(new Model(model)) : static_cast<void*>(new AnimatedModel(model));
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
					ImGui::DragFloat3("Dimensions", reinterpret_cast<float*>(&dimensions), 0.1f, 0.0f, std::numeric_limits<float>::max());
					ImGui::InputFloat3("Dimensions (fixed)", reinterpret_cast<float*>(&dimensions));
					for (int i = 0; i < 3; i++) {
						if (dimensions[i] < 0.0f) {
							dimensions[i] = 0.0f;
						}
					}
					min = -dimensions / 2.0f;
					max = dimensions / 2.0f;
				} else {
					ImGui::DragFloat3("Min", reinterpret_cast<float*>(&min), 0.1f);
					ImGui::InputFloat3("Min (fixed)", reinterpret_cast<float*>(&min));
					ImGui::DragFloat3("Max", reinterpret_cast<float*>(&max), 0.1f);
					ImGui::InputFloat3("Max (fixed)", reinterpret_cast<float*>(&max));
					for (int i = 0; i < 3; i++) {
						if (min[i] > max[i]) {
							min[i] = max[i];
						}
					}
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
					if (typeCreation->typeToCreate == SMeshColorBox) {
						box = new MeshColorBox(min, max, color);
					} else if (typeCreation->typeToCreate == SMeshRefBox) {
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

				ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::DragFloat("Height", &height, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Height (fixed)", &height);
				ImGui::DragInt("Side amount", &sideAmount, 1, 3, std::numeric_limits<int>::max());

				if (radius < 0.0f) {
					radius = 0.0f;
				}
				if (height < 0.0f) {
					height = 0.0f;
				}
				if (sideAmount < 3) {
					sideAmount = 3;
				}

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

				ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::DragFloat("Height", &height, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Height (fixed)", &height);
				ImGui::DragInt("Side amount", &sideAmount, 1, 3, std::numeric_limits<int>::max());

				if (radius < 0.0f) {
					radius = 0.0f;
				}
				if (height < 0.0f) {
					height = 0.0f;
				}
				if (sideAmount < 3) {
					sideAmount = 3;
				}

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

				ImGui::DragFloat("Width", &width, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Width (fixed)", &width);
				ImGui::DragFloat("Length", &length, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Length (fixed)", &length);

				if (width < 0.0f) {
					width = 0.0f;
				}
				if (length < 0.0f) {
					length = 0.0f;
				}

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

				ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Radius (fixed)", &radius);
				ImGui::DragInt("Precision", &precision, 1, 3, std::numeric_limits<int>::max());

				if (radius < 0.0f) {
					radius = 0.0f;
				}
				if (precision < 3) {
					precision = 3;
				}

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

				ImGui::DragFloat("Radius 1", &radiusIn, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Radius 1 (fixed)", &radiusIn);
				ImGui::DragFloat("Radius 2", &radiusOut, 0.1f, 0.0f, std::numeric_limits<float>::max());
				ImGui::InputFloat("Radius 2", &radiusOut, 0.1f);
				ImGui::DragInt("Side amount", &sideAmount, 1, 3, std::numeric_limits<int>::max());

				if (radiusIn < 0.0f) {
					radiusIn = 0.0f;
				}
				if (radiusOut < 0.0f) {
					radiusOut = 0.0f;
				}
				if (sideAmount < 3) {
					sideAmount = 3;
				}

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
			case SMeshText:
			{
				typeCreation->creationCallback(new MeshText());
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SKeyFrameAnimation:
			{
				typeCreation->creationCallback(new KeyFrameAnimation(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SSkeletalAnimation:
			{
				//todo
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SAnimationController:
			{
				static int animationType;
				if (typeCreation->typeCreationStarted) {
					animationType = 0;
				}
				if (ImGui::BeginCombo("Animation type", AnimationTypeNames[animationType].c_str())) {
					for (int i = 0; i < sizeof(AnimationTypes) / sizeof(*AnimationTypes); i++) {
						bool isSelected = i == animationType;
						if (ImGui::Selectable(AnimationTypeNames[animationType].c_str(), isSelected)) {
							animationType = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(new AnimationController(AnimationTypes[animationType], reinterpret_cast<GraphNode*>(typeCreation->arg)));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SBillboard:
			{
				static bool rescale;
				if (typeCreation->typeCreationStarted) {
					rescale = true;
				}
				ImGui::Checkbox("Rescale", &rescale);
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(new Billboard(playerCamera, reinterpret_cast<GraphNode*>(typeCreation->arg), rescale));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SSphereCollider:
			case SBoxCollider:
			{
				static bool isTrigger;
				static bool isDynamic;
				static float radius;
				static glm::vec3 dimensions;
				static glm::vec3 positionOffset;
				if (typeCreation->typeCreationStarted) {
					isTrigger = false;
					isDynamic = true;
					radius = 0.5f;
					dimensions = glm::vec3(1.0f, 1.0f, 1.0f);
					positionOffset = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				ImGui::Checkbox("Trigger", &isTrigger);
				ImGui::Checkbox("Dynamic", &isDynamic);
				switch (typeCreation->typeToCreate) {
					case SSphereCollider:
						ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
						ImGui::InputFloat("Radius (fixed)", &radius);
						if (radius < 0.0f) {
							radius = 0.0f;
						}
						break;
					case SBoxCollider:
						ImGui::DragFloat3("Dimensions", reinterpret_cast<float*>(&dimensions), 0.1f, 0.0f, std::numeric_limits<float>::max());
						ImGui::InputFloat3("Dimensions (fixed)", reinterpret_cast<float*>(&dimensions));
						for (int i = 0; i < 3; i++) {
							if (dimensions[i] < 0.0f) {
								dimensions[i] = 0.0f;
							}
						}
						break;
				}
				if (ImGui::Button("Create")) {
					Collider *collider;
					if (typeCreation->typeToCreate == SSphereCollider) {
						collider = new SphereCollider(reinterpret_cast<GraphNode*>(typeCreation->arg), isDynamic ? DYNAMIC : STATIC, isTrigger, positionOffset, radius);
					} else {
						collider = new BoxCollider(reinterpret_cast<GraphNode*>(typeCreation->arg), isDynamic ? DYNAMIC : STATIC, isTrigger, positionOffset, dimensions / 2.0f);
					}
					typeCreation->creationCallback(collider);
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SPhysicalObject:
			{
				typeCreation->creationCallback(new PhysicalObject(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SRotatingObject:
			{
				static float rotationSpeed;
				if (typeCreation->typeCreationStarted) {
					rotationSpeed = glm::radians(45.0f);
				}
				ImGui::SliderAngle("Rotation speed (degrees/s)", &rotationSpeed);
				float rotationSpeedDeg = glm::degrees(rotationSpeed);
				ImGui::InputFloat("Rotation speed (fixed)", &rotationSpeedDeg);
				rotationSpeed = glm::radians(rotationSpeedDeg);
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(new RotatingObject(rotationSpeed, reinterpret_cast<GraphNode*>(typeCreation->arg)));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SCollectableObject:
			{
				typeCreation->creationCallback(new CollectableObject(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SCollisionTest:
			{
				typeCreation->creationCallback(new CollisionTest(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SPicking:
			{
				typeCreation->creationCallback(new Picking(reinterpret_cast<GraphNode*>(typeCreation->arg), playerCamera, editedScene));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SSunController:
			{
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(new SunController(reinterpret_cast<GraphNode*>(typeCreation->arg), editedScene));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SColumnMoving:
			{
				if (ImGui::Button("Create")) {
					typeCreation->creationCallback(new ColumnMoving(reinterpret_cast<GraphNode*>(typeCreation->arg)));
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SPlayerMovement:
			{
				typeCreation->creationCallback(new PlayerMovement(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SDirLightComp:
			{
				if (lightManager->getDirAmount() != MAX_LIGHTS_OF_TYPE) {
					typeCreation->creationCallback(new DirLightComp(lightManager->addDirLight(), reinterpret_cast<GraphNode*>(typeCreation->arg)));
					editedScene->setLights(lightManager->getLights());
				}
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SSpotLightComp:
			{
				if (lightManager->getSpotAmount() != MAX_LIGHTS_OF_TYPE) {
					typeCreation->creationCallback(new SpotLightComp(lightManager->addSpotLight(), reinterpret_cast<GraphNode*>(typeCreation->arg)));
					editedScene->setLights(lightManager->getLights());
				}
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SPointLightComp:
			{
				if (lightManager->getPointAmount() != MAX_LIGHTS_OF_TYPE) {
					typeCreation->creationCallback(new PointLightComp(lightManager->addPointLight(), reinterpret_cast<GraphNode*>(typeCreation->arg)));
					editedScene->setLights(lightManager->getLights());
				}
				typeCreationsToDelete.push_back(typeCreation);
			}
			break;
			case SSun:
			{
				if (lightManager->getDirAmount() < MAX_LIGHTS_OF_TYPE - 1) {
					static glm::vec4 dawn, day, dusk, night;
					static float sunDistance, initialTime, rotationAngle;
					if (typeCreation->typeCreationStarted) {
						dawn = normalize(glm::vec4(254, 107, 0, 255));
						day = normalize(glm::vec4(173, 161, 70, 255));
						dusk = normalize(glm::vec4(0, 2, 15, 255));
						night = normalize(glm::vec4(2, 5, 18, 255));
						sunDistance = 10.0f;
						initialTime = 12.0f;
						rotationAngle = glm::radians(75.0f);
					}
					ImGui::ColorEdit4("Dawn color", reinterpret_cast<float*>(&dawn));
					ImGui::ColorEdit4("Day color", reinterpret_cast<float*>(&day));
					ImGui::ColorEdit4("Dusk color", reinterpret_cast<float*>(&dusk));
					ImGui::ColorEdit4("Night color", reinterpret_cast<float*>(&night));
					ImGui::DragFloat("Sun distance", &sunDistance, 0.1f);
					ImGui::DragFloat("Initial time", &initialTime, 0.1f);
					ImGui::SliderAngle("Rotation angle", &rotationAngle);

					if (ImGui::Button("Create")) {
						typeCreation->creationCallback(new Sun(lightManager->addDirLight(), lightManager->addDirLight(), dawn, day, dusk, night,
															   sunDistance, initialTime, rotationAngle, reinterpret_cast<GraphNode*>(typeCreation->arg)));
						editedScene->setLights(lightManager->getLights());
						typeCreationsToDelete.push_back(typeCreation);
					}
				} else {
					typeCreationsToDelete.push_back(typeCreation);
				}
			}
			break;
			case SAnimTimeSaver:
				typeCreation->creationCallback(new AnimTimeSaver(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
				break;
			case SSunTimeActivator:
				typeCreation->creationCallback(new SunTimeActivator(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
				break;
			case SCollectableWatch:
				typeCreation->creationCallback(new CollectableWatch(reinterpret_cast<GraphNode*>(typeCreation->arg)));
				typeCreationsToDelete.push_back(typeCreation);
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
	typeCreationsToDelete.clear();

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

	if (componentSelectionCallback != nullptr) {
		ImGui::Begin("SELECT COMPONENT TYPE", nullptr, 64);
		for (auto &type : creatableComponents) {
			if (ImGui::Button(SerializableTypeNames[type].c_str())) {
				componentSelectionCallback(type);
				componentSelectionCallback = nullptr;
			}
		}
		if (ImGui::Button("CANCEL")) {
			componentSelectionCallback = nullptr;
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

	if(prefabSelectionCallback != nullptr) {
		ImGui::Begin("SELECT PREFAB TYPE", nullptr, 64);
		for(int i=0;i<sizeof(PrefabNames)/sizeof(*PrefabNames);i++) {
			std::string prefabName = PrefabNames[i];
			ImGui::PushID(prefabName.c_str());
			if(ImGui::Button(prefabName.c_str())) {
				prefabSelectionCallback(Prefabs[i]);
				prefabSelectionCallback = nullptr;
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
			if (confirmationDialogCallback == nullptr && ImGui::Button("Delete the skybox")) {
				confirmationDialogCallback = [this]() {
					Skybox *skybox = editedScene->getSkybox();
					editedScene->setSkybox(nullptr);
					delete skybox;
				};
			}
		}
		ImGui::NewLine();
		showNodeAsTree(editedScene->rootNode);
		ImGui::TreePop();
		ImGui::End();

		//ImGui::Begin("Object builder", nullptr, 64);
		//if (ImGui::Button("Add box")) {
		//	appendNode(Node::createBox(glm::vec3(1), Node::getRandomColor()));
		//}
		//ImGui::End();
		static std::vector<GraphNode*> toDelete;
		for (auto &node : editedNodes) {
			ImGui::PushID(idCounter++);
			ImGui::Begin(("Node '" + node->getName() + "' - " + std::to_string(idCounter - 1)).c_str(), nullptr, 64);
			if (ImGui::Button("Close")) {
				toDelete.push_back(node);
				if (useWireframe) {
					node->removeTempRenderMode();
				}
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
	//return playerCamera;
}

void EditorScene::update(double deltaTime) {
	for (auto &pair : addRenderedNodeQueue) {
		if (pair.second == nullptr) {
			pair.second = editedScene->getRootNode();
		}
		if (pair.first->getParent() == nullptr) {
			pair.first->setParent(pair.second);
		}
		if (pair.first->getName() == "Node") {
			pair.first->setName("Node #" + std::to_string(nodeCounter++));
		}
		editedScene->addRenderedNode(pair.first, pair.second);

	}
	addRenderedNodeQueue.clear();

	if (editedScene != nullptr && updateEditedScene) {
		editedScene->update(deltaTime * timeScale);
	}

	editorCamera->getGameObject()->update(deltaTime);
	Scene::update(deltaTime);
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

void EditorScene::addTypeCreation(SerializableType type, std::function<void(void*)> creationCallback, void* arg) {
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
		creation->arg = arg;
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
	textureSelectionCallback = nullptr;
	modelSelectionCallback = nullptr;
	componentSelectionCallback = nullptr;
	meshSelectionCallback = nullptr;
	shaderTypeSelectionCallback = nullptr;

	for (auto &tc : typeCreations) {
		delete tc;
	}
	typeCreations.clear();

	if (scene != nullptr) {
		Camera *camera = scene->getCamera();
		editorCamera->getGameObject()->localTransform.setData(baseData);
		if (camera != nullptr) {
			delete playerCamera;
			playerCamera = camera;
		} else {
			playerCamera = nullptr;
			setEditorCamera(true);
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

void EditorScene::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (useEditorCamera) {
		Scene::keyboard_callback(window, key, scancode, action, mods);
	} else {
		if (editedScene != nullptr)
			editedScene->keyboard_callback(window, key, scancode, action, mods);
	}
}

void EditorScene::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (useEditorCamera) {
		Scene::mouse_callback(window, xpos, ypos);
	} else {
		if (editedScene != nullptr)
			editedScene->mouse_callback(window, xpos, ypos);
	}
}

void EditorScene::mouse_button_callback(GLFWwindow* window, int butt, int action, int mods) {
	if (useEditorCamera) {
		Scene::mouse_button_callback(window, butt, action, mods);
	} else {
		if (editedScene != nullptr)
			editedScene->mouse_button_callback(window, butt, action, mods);
	}
}

void EditorScene::toggleWireframe() {
	useWireframe = !useWireframe;
	if (useWireframe) {
		for (auto &node : editedNodes) {
			node->setTempRenderMode(GL_LINE_STRIP);
		}
	} else {
		for (auto &node : editedNodes) {
			node->removeTempRenderMode();
		}
	}
}

void EditorScene::appendNode(GraphNode* node, GraphNode* parent) {
	addRenderedNodeQueue.push_back(std::pair<GraphNode*, GraphNode*>(node, parent));
}

void EditorScene::applyPrefab(GraphNode* const node, Prefab prefab) {
	switch(prefab) {
		case PrefCache:
			BoxCollider *collider = new BoxCollider(node, DYNAMIC, true);
			AnimTimeSaver *saver = new AnimTimeSaver(node);
			Sun *sun = editedScene->rootNode->getComponentInChildren<Sun>();
			KeyFrameAnimation *anim = new KeyFrameAnimation(node);
			SunTimeActivator *activator = new SunTimeActivator(node);
			saver->setSun(sun);
			activator->addActivatableComponent(anim);
			saver->setAnimation(anim);
			activator->setSun(sun);
			saver->setDisableCollider(true);
			editedScene->addComponent(node, collider);
			editedScene->addComponent(node, activator);
			editedScene->addComponent(node, anim);
			editedScene->addComponent(node, saver);
			break;
	}
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
	if (!opened) {
		if (ImGui::Button("Open...")) {
			editedNodes.push_back(node);
			if (useWireframe) {
				node->setTempRenderMode(GL_LINE_STRIP);
			}
		}
	} else {
		if (ImGui::Button("Close...")) {
			for (auto i = editedNodes.begin(); i != editedNodes.end();) {
				if (*i == node) {
					if (useWireframe) {
						node->removeTempRenderMode();
					}
					editedNodes.erase(i);
					break;
				}
				++i;
			}
		}
	}
	ImGui::SameLine();
	if (!typeCreationExists(SGraphNode) && ImGui::Button("Add child...")) {
		addTypeCreation(SGraphNode, [this, node](void* nod) {
			if (nod != nullptr) {
				appendNode(reinterpret_cast<GraphNode*>(nod), node);
				editedNodes.push_back(reinterpret_cast<GraphNode*>(nod));
				if (useWireframe) {
					node->setTempRenderMode(GL_LINE_STRIP);
				}
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
		if(this->prefabSelectionCallback == nullptr) {
			if(ImGui::Button("Apply prefab...")) {
				prefabSelectionCallback = [this,node](Prefab prefab) {
					applyPrefab(node, prefab);
				};
			}
			ImGui::SameLine();
		}
		if (this->confirmationDialogCallback == nullptr && ImGui::Button("Delete")) {
			confirmationDialogCallback = [this, node]() {
				editedScene->removeNode(node);
				for (auto i = editedNodes.begin(); i != editedNodes.end();) {
					if (*i == node) {
						editedNodes.erase(i);
						break;
					}
					++i;
				}
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
			case EDITOR_KEY_TOGGLE_WIREFRAME:
				toggleWireframe();
				break;
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
	if (playerCamera == nullptr && !enabled) {
		setEditorCamera(true);
		return;
	}
	useEditorCamera = enabled;
	if (editedScene != nullptr) {
		editedScene->setCamera(useEditorCamera ? editorCamera : playerCamera);
	}
	cameraText->setText(enabled ? "Editor camera" : "Player camera");
}
