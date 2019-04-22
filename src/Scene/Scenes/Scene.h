#pragma once

#include "Global.h"
#include "Ui/UiElement.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Scene/OctreeNode.h"
#include "Render/LightManager.h"

#define ENABLE_FRUSTUM_CULLING

class LightManager;
struct GameFramebuffers;
class AssetManager;
class GameManager;

class Scene : public Serializable {
public:
	virtual void render();
	virtual void renderUi();
	virtual Camera *getCamera();
	void addRenderedNode(GraphNode* node, GraphNode* parent = nullptr, bool recurse = true);
	void removeNode(GraphNode* node, bool recurse = true);
	void renderNodesUsingRenderMap(Shader *shader = nullptr, bool ignoreLight=false);
	void renderNodesUsingTransparentRenderMap(Shader *shader = nullptr, bool ignoreLight = false);
	void renderUiUsingRenderMap(Shader *shader = nullptr);
	void addComponent(GraphNode* node, Component *component);
	void addToRenderMap(GraphNode *node, bool recurse = true);
	void addToRenderMap(UiElement *uiElement);
	void removeComponent(GraphNode *node, Component *component);
	void removeFromRenderMap(Renderable *renderable);
	void removeFromRenderMap(GraphNode *node, bool recurse = true);
	void removeFromRenderMap(UiElement *uiElement);
	void reinitializeRenderMap();
	virtual void keyEvent(int key, bool pressed);
	virtual void mouseEvent(int key, bool pressed);
	virtual void update(double deltaTime);
	virtual void updateWindowSize(float windowWidth, float windowHeight, float screenWidth, float screenHeight);
	virtual void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void mouse_button_callback(GLFWwindow* window, int butt, int action, int mods);
	Scene();
	virtual ~Scene();
	Lights getLights();
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	GraphNode* getRootNode() const;
protected:
	bool getKeyState(int key) const;
	bool getMouseState(int key) const;
	void addToRenderMap(GraphNode *node, bool recurse, bool checkIfExists);
	void addToRenderMap(Renderable *renderable, bool checkIfExists);
	void addToRenderMap(UiElement *uiElement, bool checkIfExists);
	void renderFromMap(bool opaque, Shader *shader, bool ignoreLight);
	std::map<ShaderType, Shader*> shaders;
	std::map<ShaderType, std::vector<Renderable*>*> renderMap;
	std::map<ShaderType, std::vector<UiElement*>*> uiRenderMap;
	std::map<ShaderType, std::vector<Renderable*>*> transparentRenderMap;
	std::vector<Renderable*> lightIgnoredObjects;
	Lights lights;
	GameFramebuffers gameFramebuffers;
	GameManager *gameManager;
	AssetManager *assetManager;
	float windowWidth, windowHeight, windowCenterX, windowCenterY, screenWidth, screenHeight;
	std::vector<UiElement*> uiElements;
	GraphNode *rootNode = new GraphNode();
	LightManager *lightManager;
};