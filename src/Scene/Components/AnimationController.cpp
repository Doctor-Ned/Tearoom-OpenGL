//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"

AnimationController::AnimationController(AnimationType _type, GraphNode *_gameObject)
:Component(_gameObject), type(_type){}

void AnimationController::startAnimation() {
    mesh = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0.5f, -0.5f, 0.0f));
    mesh->draw(transformMatrix);
}

void AnimationController::update(float msec)
{
    switch (type)
    {
        case DoorOpeningX:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.0f, 0.0f, -0.04f));
                elapsed += 0.04f;
            break;
        case DoorClosingX:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.0f, 0.0f, 0.04f));
                elapsed += 0.04f;
            break;
        case DoorOpeningY:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.0f, 0.0f, 0.04f));
            elapsed += 0.04f;
            break;
        case DoorClosingY:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.0f, 0.0f, 0.04f));
            elapsed += 0.04f;
            break;
        case SafePullOutX:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.03f, 0.0f, 0.0f));
                elapsed += 0.04f;
            break;
        case SafePullOutY:
            if(elapsed < 1.8f)
                gameObject->localTransform.Translate(glm::vec3(0.03f, 0.0f, 0.0f));
            elapsed += 0.04f;
            break;
    }

}

void AnimationController::draw()
{

}

AnimationController::~AnimationController()
{

}

