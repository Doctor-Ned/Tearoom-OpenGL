//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"

AnimationController::AnimationController(AnimationType _type, GraphNode *_gameObject, bool* f_keyPressed)
:Component(_gameObject), type(_type) {
    F_keyState = f_keyPressed;
}

void AnimationController::startAnimation() {
	animating = true;
}

void AnimationController::update(float msec)
{
    if(*F_keyState && gameObject->getHitByRay()) {startAnimation();}

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
                    gameObject->localTransform.translate(glm::vec3(0.0f, 0.0f, 0.04f));
                elapsed += 0.04f;
                break;
            case DoorClosingY:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.0f, 0.0f, 0.04f));
                elapsed += 0.04f;
                break;
            case SafePullOutX:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.03f, 0.0f, 0.0f));
                elapsed += 0.04f;
                break;
            case SafePullOutY:
                if (elapsed < 1.8f)
                    gameObject->localTransform.translate(glm::vec3(0.03f, 0.0f, 0.0f));
                elapsed += 0.04f;
                break;
        }
    }

}

AnimationController::~AnimationController()
{

}

