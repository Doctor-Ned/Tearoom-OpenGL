#pragma once
#include "Scene/Scene.h"
#include "Scene/GraphNode.h"

class MiszukScene :
	public Scene
{
public:
	GraphNode* rootNode;
	virtual void render() override;
	virtual void update(double deltaTime) override;
	GraphNode* getRootNode();
	MiszukScene();
	~MiszukScene();
};

