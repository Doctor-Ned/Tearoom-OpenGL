#pragma once
#include "Scene.h"
#include "Render/Camera.h"
#include "GLFW/glfw3.h"
#include "Ubo/UboLights.h"
#include "Ubo/UboViewProjection.h"
#include "Ubo/UboTextureColor.h"
#include "Render/GeometryShader.h"
#include "Render/Skybox.h"
#include "Render/LightManager.h"
#include "Scene/Components/LightComponents/Sun.h"

class DirLightComp;
class SpotLightComp;
class PointLightComp;
class MeshColorSphere;

const int
KEY_FORWARD = GLFW_KEY_W,
KEY_BACKWARD = GLFW_KEY_S,
KEY_LEFT = GLFW_KEY_A,
KEY_RIGHT = GLFW_KEY_D,
KEY_UP = GLFW_KEY_E,
KEY_DOWN = GLFW_KEY_Q,
KEY_FAST = GLFW_KEY_LEFT_SHIFT,
KEY_SLOW = GLFW_KEY_LEFT_CONTROL,
KEY_MOUSE_LEFT = GLFW_KEY_LEFT,
KEY_MOUSE_RIGHT = GLFW_KEY_RIGHT,
KEY_MOUSE_UP = GLFW_KEY_UP,
KEY_MOUSE_DOWN = GLFW_KEY_DOWN,
KEY_TOGGLE_MOUSE_LOCK = GLFW_KEY_SPACE,
KEY_QUIT = GLFW_KEY_ESCAPE,
KEY_INTERACT = GLFW_KEY_F;

class TestScene : public Scene {
public:
	TestScene();
	void render() override;
	void renderUi() override;
	void update(double deltaTime) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	TestScene(bool serialized);
	void keyEvent(int key, bool pressed) override;
	Skybox *skybox;
	std::vector<DirLightComp*> dirLightComps;
	std::vector<SpotLightComp*> spotLightComps;
	std::vector<PointLightComp*> pointLightComps;
	GraphNode *modelNode = nullptr;
	MeshColorSphere *pointLightSphere;
	GraphNode *sunNode;
	Sun *sun;
	friend class Serializer;
};

