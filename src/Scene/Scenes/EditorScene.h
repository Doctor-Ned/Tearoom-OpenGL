#pragma once
#include "Scene.h"

class UiText;

const int
EDITOR_KEY_TOGGLE_CAMERA = GLFW_KEY_C;

class EditorScene : public Scene {
public:
	EditorScene();
	void render() override;
	void renderUi() override;
	Camera *getCamera() override;
	void update(double deltaTime) override;
protected:
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
	bool showConfirmationDialog = false, showSaveDialog = false, showLoadDialog = false;
	std::function<void()> confirmationDialogCallback;
	Scene *editedScene = nullptr;
	Serializer *serializer;
	friend class Serializer;
};

