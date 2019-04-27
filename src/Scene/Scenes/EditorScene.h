#pragma once
#include "Scene.h"

class UiText;

const int
EDITOR_KEY_TOGGLE_CAMERA = GLFW_KEY_C;

enum TypeToCreate {
	TTCNone,
	TTCGraphNode,
	TTCSkybox,
	TTCComponent,
	TTCMesh
};

struct TypeCreation {
	SerializableType typeToCreate = SNone;
	bool typeCreationStarted = true;
	std::function<void(void*)> creationCallback = nullptr;
	TypeToCreate ttc = TTCNone;
};

class EditorScene : public Scene {
public:
	EditorScene();
	void render() override;
	void renderUi() override;
	Camera *getCamera() override;
	void update(double deltaTime) override;
	void addEditedNode(GraphNode *node);
//protected:
	void showTextureGui(std::string &texture);
	void addTypeCreation(SerializableType type, std::function<void(void*)> creationCallback);
	bool typeCreationExists(SerializableType type);
	void deleteTypeCreation(TypeCreation *typeCreation);
	bool doesAnyChildContain(GraphNode *node, GraphNode *target);
	void loadTexturesModels();
	void setEditedScene(Scene *scene, bool deletePrevious = true);
	std::vector<GraphNode*> editedNodes;
	bool updateEditedScene = true;
	float timeScale = 1.0f;
	int nodeCounter = 1;
	int idCounter = 1;
	void appendNode(GraphNode *node, GraphNode *parent = nullptr);
	void showNodeAsTree(GraphNode *node);
	void keyEvent(int key, bool pressed) override;
	bool useEditorCamera = true;
	void setEditorCamera(bool enabled);
	Camera *editorCamera, *playerCamera = nullptr;
	UiText *cameraText;
	SerializableType creatableTypes[2] = { SGraphNode, SSkybox };
	SerializableType creatableComponents[14] = { SAnimation, SAnimationController, SBillboard, SBoxCollider, SSphereCollider, SPhysicalObject, SRotatingObject, SCollectableObject, SPicking, SPlayerMovement,
		SDirLightComp, SSpotLightComp, SPointLightComp, SSun};
	SerializableType creatableMeshes[15] = { SModel, SMeshBox, SMeshColorBox, SMeshColorCone, SMeshColorCylinder, SMeshColorPlane, SMeshColorSphere, SMeshColorTorus, SMeshCone, SMeshCylinder, SMeshPlane, SMeshRefBox, SMeshRefSphere,
	SMeshSphere, SMeshTorus};
	bool showSaveDialog = false, showLoadDialog = false;
	std::function<void()> confirmationDialogCallback = nullptr;
	std::function<void(GraphNode*)> nodeSelectionCallback = nullptr;
	std::function<void(Texture)> textureSelectionCallback = nullptr;
	std::function<void(ShaderType)> shaderTypeSelectionCallback = nullptr;
	std::function<void(SerializableType)> meshSelectionCallback = nullptr;
	std::function<void(std::string)> modelSelectionCallback = nullptr;
	std::vector<TypeCreation*> typeCreations;
	Scene *editedScene = nullptr;
	Serializer *serializer;
	friend class Serializer;
	std::vector<std::string> textures;
	std::vector<std::string> models;
};

