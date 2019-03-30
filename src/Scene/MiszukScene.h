#pragma once
#include "Scene/Scene.h"
#include "Scene/GraphNode.h"

class MiszukScene :
	public Scene
{
public:
	GraphNode* rootNode;
	void render() override;
	void update(double deltaTime) override;
	void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) override;
	GraphNode* getRootNode();
	MiszukScene();
	~MiszukScene();
};

