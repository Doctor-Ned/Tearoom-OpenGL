//
// Created by MB on 4/3/2019.
//

#ifndef TEAROOM_ANIMATION_H
#define TEAROOM_ANIMATION_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"

class OutroCutscene;

class AnimationController : public Component {

protected:
	friend class Serializer;
	AnimationController() {}
	Animation* anim = nullptr;
	OutroCutscene *outro = nullptr;
	int doorID = 0;
	void renderGui() override;
public:
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	AnimationController(GraphNode * _gameObject);
	AnimationController(GraphNode* _gameObject, int doorID, Animation* anim);
	void update(float msec) override;
	int getDoorID();
	void open();
};

#endif //TEAROOM_ANIMATION_H
