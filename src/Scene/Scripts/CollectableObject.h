//
// Created by MB on 4/20/2019.
//

#ifndef TEAROOM_COLLECTABLEOBJECT_H
#define TEAROOM_COLLECTABLEOBJECT_H

#include <Ui/UiPlane.h>
#include "Scene/Components/Component.h"
#include "Render/Renderable.h"
#include "Mesh/Mesh.h"
#include "Ui/UiButton.h"

enum ItemType {
	Letter,
	NormalItem,
	Photo,
	DoorKey
};

const ItemType ItemTypes[] = {
	Letter,
	NormalItem,
	Photo,
	DoorKey 
};

const std::string ItemTypeNames[]{
	"Letter",
	"NormalItem",
	"Photo",
	"DoorKey"
};

class CollectableObject : public Component {

public:
	virtual ~CollectableObject();
	CollectableObject(GraphNode* _gameObject, ItemType i_type, std::string icon, glm::vec2 iconPos, glm::vec2 iconSize, std::string desc);
	CollectableObject(GraphNode* _gameObject, ItemType i_type, std::string icon, glm::vec2 iconPos, glm::vec2 iconSize, std::string desc, std::string preview, glm::vec2 previewPos, glm::vec2 previewSize);
    CollectableObject(GraphNode* _gameObject, ItemType i_type, std::string icon, glm::vec2 iconPos, glm::vec2 iconSize, std::string desc, std::string preview, glm::vec2 previewPos, glm::vec2 previewSize, int doorID);
    void takeObject();
	void update(float msec) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
    bool getIsTaken() const;
    UiText* getDescription();
    ItemType getI_type() const;
    UiPlane *getIcon() const;
    UiButton* getButton();
    void setButton(UiButton* button);
    UiPlane* getPreview();
    int getDoorID();

protected:
	void renderGui() override;
	CollectableObject(GraphNode* _gameObject);
	void initializeUiPlanes();
    int doorID = 0;
	std::string iconResource, previewResource;
	glm::vec2 iconPos, iconSize, previewPos, previewSize;
    UiPlane* icon = nullptr;
    ItemType i_type;
    bool isTaken = false;
    UiText* desctext = nullptr;
    UiButton* itemButton = nullptr;
    UiPlane* preview = nullptr;
	std::string desc;
    bool isHitByRay = false;
	bool fKeyState = false;
	CollectableObject(){}
	friend class Serializer;
};

#endif //TEAROOM_COLLECTABLEOBJECT_H
