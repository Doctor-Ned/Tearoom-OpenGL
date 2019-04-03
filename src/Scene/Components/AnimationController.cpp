//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"

AnimationController::AnimationController(Mesh *mesh) {
    this->mesh = mesh;
}

void AnimationController::calculatePath() {

}

void AnimationController::startAnimation() {
    //calculatePath()
    mesh->draw(newPosition);
}