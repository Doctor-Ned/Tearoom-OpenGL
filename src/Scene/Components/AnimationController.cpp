//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"

AnimationController::AnimationController(AnimationType _type, GraphNode *_gameObject)
:Component(_gameObject, "Animation controller"), type(_type) {
}

AnimationController::AnimationController(AnimationType _type, GraphNode *_gameObject, int doorID, Animation* anim)
        :Component(_gameObject, "Animation controller"), type(_type), doorID(doorID), anim(anim) {
}

void AnimationController::startAnimation() {
	animating = true;
}

void AnimationController::update(float msec)
{

}

void AnimationController::playAnimation() {
	anim->play();
}

SerializableType AnimationController::getSerializableType() {
	return SAnimationController;
}

Json::Value AnimationController::serialize(Serializer* serializer) {
	Json::Value root = Component::serialize(serializer);
	root["animating"] = animating;
	root["type"] = static_cast<int>(type);
	root["elapsed"] = elapsed;
	return root;
}

void AnimationController::deserialize(Json::Value& root, Serializer* serializer) {
	Component::deserialize(root, serializer);
	animating = root["animating"].asBool();
	type = static_cast<AnimationType>(root["type"].asInt());
	elapsed = root["elapsed"].asFloat();
}

AnimationController::~AnimationController()
{

}

AnimationType AnimationController::getType() const {
    return type;
}

int AnimationController::getDoorID() {
    return doorID;
}

