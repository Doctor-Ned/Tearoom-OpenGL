#ifndef MISZUK_SCENE
#define MISZUK_SCENE

#include "Scene.h"
#include <map>
#include "Ui/UiColorPlane.h"

class MiszukScene : public Scene {
public:
	MiszukScene();
	~MiszukScene();
protected:
	void keyEvent(int key, bool pressed) override;
	GraphNode* player;
};

#endif