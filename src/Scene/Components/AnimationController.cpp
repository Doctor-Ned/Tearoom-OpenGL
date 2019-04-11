//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"
#include "Scene/GraphNode.h"
#include "Scene/GameManager.h"

AnimationController::AnimationController(AnimationType _type, GraphNode *_gameObject)
:Component(_gameObject), type(_type)
{

}

void AnimationController::calculatePath() {

}

void AnimationController::startAnimation() {

    mesh = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    glm::mat4 transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, glm::vec3(0.5f, -0.5f, 0.0f));
    mesh->draw(transformMatrix);

}

AnimationController::~AnimationController()
{

}

void AnimationController::update(float msec)
{
    if(type == DoorOpening)
    {
        if(elapsed < 1.8f && )
        gameObject->localTransform.Translate(glm::vec3(0.0f, 0.0f, -0.04f));
        //rootNode->getChild(0)->localTransform.Translate(glm::vec3(0.03f, 0.0f, 0.0f));
        elapsed += 0.04f;
    }
    if(type == SafePullOut)
    {

    }
}

void AnimationController::draw()
{

}

