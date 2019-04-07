//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"



AnimationController::AnimationController(AnimationType animType, Mesh *mesh, GraphNode* _gameObject):Component(_gameObject) {
    this->mesh = mesh;
    this->animType = animType;
}

void AnimationController::calculatePath() {

}

void AnimationController::startAnimation() {
    //box for testing
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

}

void AnimationController::draw()
{

}

