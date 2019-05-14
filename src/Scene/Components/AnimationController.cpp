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

void AnimationController::startAnimation() {
	animating = true;
}

void AnimationController::update(float msec)
{
    if(animating) {
        switch (type) {
            case DoorOpeningX:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.0f, 0.0f, -0.04f));
                elapsed += 0.01f;
                break;
            case DoorClosingX:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.0f, 0.0f, 0.04f));
                elapsed += 0.04f;
                break;
            case DoorOpeningY:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.0f, 0.04f, 0.0f));
                elapsed += 0.04f;
                break;
            case DoorClosingY:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.0f, -0.04f, 0.00f));
                elapsed += 0.04f;
                break;
            case SafePullOutX:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.03f, 0.0f, 0.0f));
                elapsed += 0.04f;
                break;
            case SafePullOutY:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(-0.03f, 0.0f, 0.0f));
                elapsed += 0.04f;
                break;
        }
    }

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

