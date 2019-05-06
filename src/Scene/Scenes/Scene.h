#pragma once

#include "Global.h"
#include "Ui/UiElement.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"
#include "Scene/OctreeNode.h"
#include "Render/LightManager.h"
#include "Render/Skybox.h"
#include "Ui/UiText.h"
#include "Ui/UiColorPlane.h"
#include "Ui/UiPlane.h"

#define ENABLE_FRUSTUM_CULLING

class LightManager;
struct GameFramebuffers;
class AssetManager;
class GameManager;

class Scene : public Serializable {
public:
	virtual void render();
	void renderUiElement(UiElement* uiElement);
	virtual void renderUi();
	virtual Camera *getCamera();
	void setCamera(Camera *camera);
	void addRenderedNode(GraphNode* node, GraphNode* parent = nullptr, bool recurse = true);
	void removeNode(GraphNode* node, bool recurse = true);
	void renderNodesUsingRenderMap(Shader *shader = nullptr, bool ignoreLight = false, bool frustumCulling = true, bool ignoreRefractive=false);
	void renderNodesUsingTransparentRenderMap(Shader *shader = nullptr, bool ignoreLight = false, bool frustumCulling=true, bool ignoreRefractive=false);
	void addComponent(GraphNode* node, Component *component);
	void addToRenderMap(GraphNode *node, bool recurse = true);
	void addToRenderMap(Renderable *renderable);
	void updateRenderable(Renderable *renderable, bool addIfNotFound = true);
	void removeComponent(GraphNode *node, Component *component);
	void removeFromRenderMap(Renderable *renderable);
	void removeFromRenderMap(GraphNode *node, bool recurse = true);
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
	virtual Skybox *getSkybox();
	void setSkybox(Skybox *skybox);
	//inventory
	std::vector<UiPlane*>* getObjectRepresentations();
	UiElement* getUiRoot();
	void setLights(Lights lights);

protected:
	bool getCursorLocked() const;
	void setCursorLocked(bool locked) const;
	bool getKeyState(int key) const;
	bool getMouseState(int key) const;
	void addToRenderMap(GraphNode *node, bool recurse, bool checkIfExists);
	void addToRenderMap(Renderable *renderable, bool checkIfExists);
	void renderFromMap(bool opaque, Shader *shader, bool ignoreLight, bool frustumCulling = true, bool ignoreRefractive = false);
	std::map<ShaderType, Shader*> shaders;
	std::map<ShaderType, std::vector<Renderable*>*> renderMap;
	std::map<ShaderType, std::vector<Renderable*>*> transparentRenderMap;
	std::vector<Renderable*> lightIgnoredObjects;
	UboLights *uboLights;
	UboTextureColor *uboTextureColor;
	UboViewProjection *uboViewProjection;
	Lights lights;
	Camera *camera = nullptr;
	std::vector<Shader*> updatableShaders;
	GameFramebuffers gameFramebuffers;
	GameManager *gameManager;
	AssetManager *assetManager;
	float windowWidth, windowHeight, windowCenterX, windowCenterY, screenWidth, screenHeight;
	UiElement *rootUiElement;

	//inventory
	std::vector<UiPlane*> objectRepresentasions;
	//-----------

	GraphNode *rootNode;
	LightManager *lightManager;
	glm::mat4 projection;
	const float BASE_MOVEMENT_SPEED = 1.0f;
	float movementSpeed = BASE_MOVEMENT_SPEED;
	float mouseX, mouseY;
	Skybox *skybox = nullptr;
private:
	float mouseMovementX, mouseMovementY;
	bool initMouse = true;
	friend class EditorScene;
};