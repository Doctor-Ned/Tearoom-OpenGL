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

class AnimationController: public Component
{

protected:
	friend class Serializer;
	AnimationController(){}
	bool animating = false;
    AnimationType type;
    glm::float32 elapsed = 0.0f;

public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
    virtual ~AnimationController();
    AnimationController(AnimationType _type, GraphNode* _gameObject);
    void update(float msec) override;
    void startAnimation();
};

#endif //TEAROOM_ANIMATION_H
