//
// Created by MB on 4/3/2019.
//

#include "AnimationController.h"
#include "Mesh/MeshColorBox.h"



AnimationController::AnimationController(AnimationType animType, Mesh *mesh, GraphNode* _gameObject):Component(_gameObject) {
    this->mesh = mesh;
    this->animType = animType;
	//box for testing
	mesh = new MeshColorBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void AnimationController::calculatePath() {

}

ShaderType AnimationController::getShaderType() {
	return ShaderType();
}

void AnimationController::startAnimation() {
}

AnimationController::~AnimationController()
{

}

void AnimationController::update(float msec)
{
	transformMatrix = glm::mat4(1.0f);
	transformMatrix = glm::translate(transformMatrix, glm::vec3(0.5f, -0.5f, 0.0f));
}

void AnimationController::updateDrawData() {
	
}

void AnimationController::drawSelf(Shader * shader) {
	mesh->draw(shader, transformMatrix);
}

