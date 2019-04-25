#ifndef PICKING_H
#define PICKING_H

#include <Ui/UiPlane.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"
#include "Ui/UiPlane.h"

class Camera;
class Picking : public Component
{
public:
	Picking(GraphNode* _gameObject, const std::string& name, Camera* cam, Scene* scene);
	void update(float msec) override;
	 ~Picking() override;
	 SerializableType getSerializableType() override;
	 Json::Value serialize(Serializer *serializer) override;
	 void deserialize(Json::Value &root, Serializer* serializer) override;
private:
	Scene* scene;
	UiPlane* hud;
    bool inventoryUI = false;
	Camera* camera;
	std::vector<GraphNode*> inventory;
	Picking(){}
	friend class Serializer;
};

#endif
