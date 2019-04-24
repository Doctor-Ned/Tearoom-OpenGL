#pragma once
#include "Scene.h"

class UiText;

class EditorScene : public Scene {
public:
	EditorScene();
	void render() override;
	void renderUi() override;
	Camera *getCamera() override;
	void update(double deltaTime) override;
protected:
	void keyEvent(int key, bool pressed) override;
	bool useEditorCamera = true;
	void setEditorCamera(bool enabled);
	Camera *editorCamera, *playerCamera;
	UiText *cameraText;
	friend class Serializer;
};

