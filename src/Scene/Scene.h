#ifndef SCENE_H
#define SCENE_H

#include "Headers.h"
#include "Ui/UiElement.h"
#include "GraphNode.h"

class SceneManager;

class Scene {
public:
	virtual void render() = 0;
	virtual void update(double deltaTime) = 0;
	virtual void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	virtual void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	Scene();
	virtual ~Scene();
protected:
	SceneManager *sceneManager;
	float windowWidth, windowHeight, windowCenterX, windowCenterY, screenWidth, screenHeight;
	std::vector<UiElement*> uiElements;
	GraphNode *rootNode = new GraphNode();
};

#endif
