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

class EditorScene : public Scene {
public:
	EditorScene();
	void render() override;
	void renderUi() override;
	Camera *getCamera() override;
	void update(double deltaTime) override;
protected:
	bool doesAnyChildContain(GraphNode *node, GraphNode *target);
	void setCreationTarget(SerializableType type, std::function<void(void*)> creationCallback = nullptr);
	void loadTexturesModels();
	void setEditedScene(Scene *scene, bool deletePrevious = true);
	std::vector<GraphNode*> editedNodes;
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
	SerializableType typeToCreate = SNone;
	TypeToCreate ttc = TTCNone;
	bool showConfirmationDialog = false, showSaveDialog = false, showLoadDialog = false;
	std::function<void(void*)> creationCallback = nullptr;
	std::function<void()> confirmationDialogCallback = nullptr;
	std::function<void(GraphNode*)> nodeSelectionCallback = nullptr;
	Scene *editedScene = nullptr;
	Serializer *serializer;
	friend class Serializer;
	std::vector<std::string> textures;
	std::vector<std::string> models;
};

