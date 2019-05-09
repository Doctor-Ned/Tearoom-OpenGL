//
// Created by MB on 4/28/2019.
//

#include "SunController.h"

SunController::SunController(GraphNode* _gameObject, Scene* scene, const std::string& name)
:Component(_gameObject, name), scene(scene){
    clockFace = new UiPlane("res/textures/clockFace.png", glm::vec2(50.0f, 580.0f), glm::vec2(200.0f,200.0f), Left);
    clockHand = new UiPlane("res/textures/clockHand.png", clockFace->getPosition() + clockFace->getSize()/2.0f, glm::vec2(60.0f,130.0f), Center);
    clockHand->setRotationAnchor(Center);
    scene->getUiRoot()->addChild(clockFace);
    scene->getUiRoot()->addChild(clockHand);
}

SunController::~SunController() {
}

//methods for sunlight control
void SunController::moveSunBackwards() {}
void SunController::moveSunForward() {}


void SunController::update(float msec) {
    GameManager *gameManager = GameManager::getInstance();

    if(gameManager->getKeyState(GLFW_KEY_1))
    {
        clockHand->localTransform.rotateZ(-0.01f);
        moveSunBackwards();
    }

    if(gameManager->getKeyState(GLFW_KEY_2))
    {
        clockHand->localTransform.rotateZ(0.01f);
        moveSunForward();
    }
}
//SERIALIZATION
/*
SerializableType SunController::getSerializableType() {

}

Json::Value SunController::serialize(Serializer* serializer) {
    return null;

}

void SunController::deserialize(Json::Value& root, Serializer* serializer) {

}
*/