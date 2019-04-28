#ifndef MISZUK_SCENE
#define MISZUK_SCENE

#include "Scene.h"
#include <map>
#include "Render/Camera.h"
#include "Ui/UiColorPlane.h"

class MiszukScene : public Scene {
public:
	void render() override;
	void renderUi() override;
	void update(double deltaTime) override;
	MiszukScene();
	~MiszukScene();
protected:
	void keyEvent(int key, bool pressed) override;
	GraphNode* player;
};

#endif