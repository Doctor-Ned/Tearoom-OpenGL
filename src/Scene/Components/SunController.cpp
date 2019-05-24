//
// Created by MB on 4/28/2019.
//

#include "SunController.h"
#include "Ui/UiPlane.h"

SunController::SunController(GraphNode* _gameObject, Scene* scene, const std::string& name)
:Component(_gameObject, name), scene(scene){
    clockFace = new UiPlane("res/textures/clockFace.png", glm::vec2(50.0f, 580.0f), glm::vec2(200.0f,200.0f), Left);
    clockHand = new UiPlane("res/textures/clockHand.png", clockFace->getPosition() + clockFace->getSize()/2.0f, glm::vec2(60.0f,130.0f), Center);
    clockBack = new UiPlane("res/textures/clockBack.png",  glm::vec2(-24.0f, 583.0f), glm::vec2(345.0f,350.0f), Left);
    clockHand->setRotationAnchor(Center);
    clockFace->setActive(false);
    clockHand->setActive(false);
    clockBack->setActive(false);

    scene->getUiRoot()->addChild(clockBack);
    scene->getUiRoot()->addChild(clockFace);
    scene->getUiRoot()->addChild(clockHand);

    GameManager::getInstance()->addKeyCallback(GLFW_KEY_1, true, [this]()
    {
        clockFace->setActive(true);
        clockHand->setActive(true);
        clockBack->setActive(true);
    });
    GameManager::getInstance()->addKeyCallback(GLFW_KEY_2, true, [this]()
    {
        clockFace->setActive(true);
        clockHand->setActive(true);
        clockBack->setActive(true);
    });
    GameManager::getInstance()->addKeyCallback(GLFW_KEY_1, false, [this]()
    {
        clockFace->setActive(false);
        clockHand->setActive(false);
        clockBack->setActive(false);
    });
    GameManager::getInstance()->addKeyCallback(GLFW_KEY_2, false, [this]()
    {
        clockFace->setActive(false);
        clockHand->setActive(false);
        clockBack->setActive(false);
    });
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