#ifndef PICKING_H
#define PICKING_H

#include <Ui/UiColorPlane.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"
#include "Ui/UiCanvas.h"
#include <cstring>
#include <Ui/UiButton.h>

class Camera;
class Picking : public Component
{
public:
	Picking(GraphNode* _gameObject, Camera* camera, Scene* scene, const std::string& name = "Picking");
	void update(float msec) override;
	 ~Picking() override;
	 SerializableType getSerializableType() override;
	 Json::Value serialize(Serializer *serializer) override;
	 void deserialize(Json::Value &root, Serializer* serializer) override;
	 void showInventoryUi();
	 void hideInventoryUi();
	 bool getSwitch();
	 void setSwitch(bool ifShown);
     Scene *getScene() const;
     UiCanvas *getInventoryCanvas() const;

private:
	Scene* scene;
    bool inventoryUI = false;
	UiCanvas *inventoryCanvas, *encouragementCanvas;
    Camera* camera;
	std::vector<GraphNode*> inventory;
	Picking(){}
	friend class Serializer;
	UiColorPlane* encouragementBackground;
	UiText* encouragementPick;
	UiText* encouragementActivate;
    UiButton* itemsButton;
    UiButton* letterButton;
    UiButton* photoButton;
	UiPlane* photosInventory;
	UiPlane* itemsInventory;
	UiPlane* letterInventory;

	UiText* itemDesc;

};

#endif
