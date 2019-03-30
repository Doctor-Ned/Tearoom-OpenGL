#include "MiszukScene.h"
#include "Scene/SceneManager.h"
#include "Mesh/Model.h"

void MiszukScene::render()
{
	rootNode->draw();
	SceneManager::getInstance()->getTextRenderer()->renderText("Testowe renderowanie tekstu", windowCenterX, windowCenterY);
}

void MiszukScene::update(double deltaTime)
{
	rootNode->update(deltaTime);
}

GraphNode* MiszukScene::getRootNode()
{
	return rootNode;
}


MiszukScene::MiszukScene()
{
	rootNode = new GraphNode();
	Model* statek = new Model("res/models/sphere/sphere.obj");
	GraphNode* spaceShip = new GraphNode(statek);
	rootNode->addChild(spaceShip);
}


MiszukScene::~MiszukScene()
{
	delete rootNode;
}


