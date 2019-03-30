#pragma once
#include "Scene/Scene.h"

class MiszukScene : public Scene {
public:
	void render() override;
	void update(double deltaTime) override;
	MiszukScene();
	~MiszukScene();
};

