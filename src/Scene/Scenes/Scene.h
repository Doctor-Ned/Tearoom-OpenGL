#pragma once

#include "Global.h"
#include "Ui/UiElement.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Scene/OctreeNode.h"

class LightManager;
struct GameFramebuffers;
class AssetManager;
class GameManager;

class Scene {
public:
	virtual void render();
	virtual void renderUi();
	void addRenderedNode(GraphNode* node, GraphNode* parent = nullptr, bool recurse = true);
	void removeNode(GraphNode* node, bool recurse = true);
	void renderNodesUsingRenderMap(Shader *shader = nullptr, bool ignoreLight=false);
	void renderNodesUsingTransparentRenderMap(Shader *shader = nullptr, bool ignoreLight = false);
	void addComponent(GraphNode* node, Component *component);
	void addToRenderMap(GraphNode *node, bool recurse = true);
	void removeComponent(GraphNode *node, Component *component);
	void removeFromRenderMap(Renderable *renderable);
	void removeFromRenderMap(GraphNode *node, bool recurse = true);
	void reinitializeRenderMap();
	virtual void update(double deltaTime);
	virtual void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	virtual void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	Scene();
	virtual ~Scene();
protected:
	bool getKeyState(int key);
	void setKeyState(int key, bool pressed);
	virtual void keyEvent(int key, bool pressed);
	std::map<int, bool> keyStates;
	void addToRenderMap(GraphNode *node, bool recurse, bool checkIfExists);
	void addToRenderMap(Renderable *renderable, bool checkIfExists);
	void renderFromMap(bool opaque, Shader *shader, bool ignoreLight);
	std::map<ShaderType, Shader*> shaders;
	std::map<ShaderType, std::vector<Renderable*>*> renderMap;
	std::map<ShaderType, std::vector<Renderable*>*> transparentRenderMap;
	GameFramebuffers gameFramebuffers;
	GameManager *gameManager;
	AssetManager *assetManager;
	float windowWidth, windowHeight, windowCenterX, windowCenterY, screenWidth, screenHeight;
	std::vector<UiElement*> uiElements;
	GraphNode *rootNode = new GraphNode();
	LightManager *lightManager;
};