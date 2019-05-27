//
// Created by MB on 4/3/2019.
//

#ifndef TEAROOM_ANIMATION_H
#define TEAROOM_ANIMATION_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"

enum AnimationType {
	DoorOpeningX,
	DoorClosingX,
	DoorOpeningY,
	DoorClosingY,
	SafePullOutX,
	SafePullOutY
};

const AnimationType AnimationTypes[6] = {
	DoorOpeningX,
	DoorClosingX,
	DoorOpeningY,
	DoorClosingY,
	SafePullOutX,
	SafePullOutY
};

const std::string AnimationTypeNames[6] = {
	"DoorOpeningX",
	"DoorClosingX",
	"DoorOpeningY",
	"DoorClosingY",
	"SafePullOutX",
	"SafePullOutY"
};

class AnimationController : public Component {

protected:
	friend class Serializer;
	AnimationController() {}
	bool animating = false;
	AnimationType type;
	int doorID = 0;
	glm::float32 elapsed = 0.0f;
public:
	AnimationType getType() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	virtual ~AnimationController();
	AnimationController(AnimationType _type, GraphNode* _gameObject);
	AnimationController(AnimationType _type, GraphNode* _gameObject, int doorID);
	void update(float msec) override;
	int getDoorID();
	void startAnimation();
};

#endif //TEAROOM_ANIMATION_H
