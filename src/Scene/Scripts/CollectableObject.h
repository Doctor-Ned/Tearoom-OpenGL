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
	CollectableObject(GraphNode* _gameObject, Camera* camera);
	CollectableObject(GraphNode* _gameObject, Camera* camera, ItemType i_type, UiPlane* icon, std::string desc);
	void takeObject();
	void leaveObject();
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

protected:
    UiPlane* icon;
    ItemType i_type;
    bool isTaken = false;
    UiText* desctext;
    UiButton* itemButton;
    Camera* camera;
    bool isHitByRay = false;
	bool fKeyState = false;
	CollectableObject(){}
	friend class Serializer;
};

#endif //TEAROOM_COLLECTABLEOBJECT_H
