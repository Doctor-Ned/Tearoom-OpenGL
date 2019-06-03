#ifndef PICKING_H
#define PICKING_H

#include <Ui/UiColorPlane.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"
#include "Ui/UiCanvas.h"
#include <cstring>
#include <Ui/UiButton.h>
#include "CollectableObject.h"

class Camera;
class Picking : public Component {
public:
	Picking(GraphNode* _gameObject, Camera* camera, Scene* scene, const std::string& name = "Picking");
	Picking(GraphNode* _gameObject, const std::string& name = "Picking");
	void update(float msec) override;
	~Picking() override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer *serializer) override;
	void deserialize(Json::Value &root, Serializer* serializer) override;
	void showInventoryUi();
	void hideInventoryUi();
	void hidePreview();
	void showPreview();
	bool getSwitch();
	void setSwitch(bool ifShown);
	Scene *getScene() const;
	UiCanvas *getInventoryCanvas() const;
	void setCamera(Camera *camera);
	Camera* getCamera();
	void placeInGrid(ItemType itype, UiCanvas* canvas);
	void setShowHint(bool showHint);
	bool getShowHint();
private:
	void renderGui() override;
	void initialize();
	void collect(CollectableObject *collectable);
	Scene* scene;
	float distance = 1.0f;
	bool inventoryUI = false;
	bool showHint = false;
	UiCanvas *encouragementCanvas, *previewCanvas, *currentCanvas;
	UiCanvas *letterInventoryCanvas, *photosInventoryCanvas, *itemsInventoryCanvas;
	Camera* camera;
	std::vector<GraphNode*> inventory;
	Picking() {}
	friend class Serializer;
	UiColorPlane* encouragementBackground;
	UiText *encouragementPick, *encouragementActivate;
	UiButton* itemsButton;
	UiButton* letterButton;
	UiButton* photoButton;
	UiPlane* photosInventory;
	UiPlane* itemsInventory;
	UiPlane* letterInventory;
	UiColorPlane* descBackground;
	GraphNode* currentInteraction = nullptr;
};

#endif
