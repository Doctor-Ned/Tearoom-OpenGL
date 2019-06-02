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

class CollectableObject : public Component {

public:
	virtual ~CollectableObject();
	CollectableObject(GraphNode* _gameObject);
	CollectableObject(GraphNode* _gameObject, ItemType i_type, UiPlane* icon, std::string desc, UiPlane* preview);
    CollectableObject(GraphNode* _gameObject, ItemType i_type, UiPlane* icon,
            std::string desc, UiPlane* preview, int doorID);

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
    int doorID = 0;
    UiPlane* icon = nullptr;
    ItemType i_type;
    bool isTaken = false;
    UiText* desctext = nullptr;
    UiButton* itemButton;
    UiPlane* preview = nullptr;
    bool isHitByRay = false;
	bool fKeyState = false;
	CollectableObject(){}
	friend class Serializer;
};

#endif //TEAROOM_COLLECTABLEOBJECT_H
