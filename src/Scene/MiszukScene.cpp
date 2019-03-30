#include "MiszukScene.h"
#include "Scene/SceneManager.h"
#include "Mesh/Model.h"

void MiszukScene::render()
{
	rootNode->draw();
	SceneManager::getInstance()->getTextRenderer()->renderText("Testowe renderowanie tekstu", 1280/2, 720/2);
}

void MiszukScene::update(double deltaTime)
{
	rootNode->update(deltaTime);
}

void MiszukScene::updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight) {
	
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


