//
// Created by MB on 4/13/2019.
//

#include "PhysicalObject.h"
#include "Scene/GraphNode.h"


PhysicalObject::PhysicalObject(GraphNode* _gameObject)
:Component(_gameObject){}

void PhysicalObject::update(float msec)
{
    //TODO condition - checking if there is a ground
    //if()
   // {
        gameObject->localTransform.translate(glm::vec3(0.0f,-0.07f,0.0f));
   // }
}

bool PhysicalObject::castRayDown() {
    return false;
}

PhysicalObject::~PhysicalObject() {

}

void PhysicalObject::updateDrawData() {}
void PhysicalObject::drawSelf(Shader *shader) {
    if(isFalling) {
    }
}

ShaderType PhysicalObject::getShaderType() {
    return STTexture;
}

bool PhysicalObject::isOpaque() {
	return mesh == nullptr || mesh->isOpaque();
}
