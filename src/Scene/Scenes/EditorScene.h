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
	int nodeCounter = 1;
	void appendNode(GraphNode *node, GraphNode *parent = nullptr);
	void showNodeAsTree(GraphNode *node) const;
	void keyEvent(int key, bool pressed) override;
	bool useEditorCamera = true;
	void setEditorCamera(bool enabled);
	Camera *editorCamera, *playerCamera;
	UiText *cameraText;
	friend class Serializer;
};

