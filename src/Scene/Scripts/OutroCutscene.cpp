//
// Created by MB on 6/11/2019.
//

#include "OutroCutscene.h"
#include <Scene/SoundSystem.h>
#include "CollectableObject.h"
#include "Picking.h"
#include "Scene/Components/SunController.h"
#include "Serialization/Serializer.h"

OutroCutscene::OutroCutscene(Scene* scene, SunController *sunController, GraphNode* player) : Component(player, "Intro Cutscene"), sunController(sunController) {
    this->scene = scene;
    this->player = player;
    initialize();
}

void OutroCutscene::runOutro() {
    gameManager->setCursorLocked(false);
    player->getComponent<Picking>()->hidePreview();

    if (sunController != nullptr) {
        sunController->setComponentActive(false);
        sunController->setClockVisibility(false);
    }
    run = true;
}

void OutroCutscene::setMenuCallbacks() {
    quit->addHoverCallback([this]() {
        quitHover->setOpacity(0.3f);
    });
    quit->addLeaveCallback([this]() {
        quitHover->setOpacity(0.0f);
    });
    quit->addClickCallback([this]() {

    });

    playAgain->addHoverCallback([this]() {
        playAgainHover->setOpacity(0.3f);
    });
    playAgain->addLeaveCallback([this]() {
        playAgainHover->setOpacity(0.0f);
    });
    
    playAgain->addClickCallback([this]() {

    });
    std::cout<<"aaa"<<std::endl;

}

void OutroCutscene::initialize() {
    UiElement *root = scene->getUiRoot();
    mainCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
    root->addChild(mainCanvas);
    mainCanvas->addChild(backgroundPlane);
    mainCanvas->addChild(paperTexture);
    mainCanvas->addChild(text1);
    mainCanvas->addChild(text2);
    mainCanvas->addChild(text3);
    mainCanvas->addChild(text4);
    mainCanvas->addChild(text5);
    mainCanvas->addChild(transitionPlane);
    mainCanvas->addChild(playAgainWindow);

    mainCanvas->addChild(quit);
    mainCanvas->addChild(playAgain);
    mainCanvas->addChild(quitHover);
    mainCanvas->addChild(playAgainHover);

    transitionPlane->setOpacity(1.0f);
    text1->setOpacity(0.0f);
    text2->setOpacity(0.0f);
    text3->setOpacity(0.0f);
    text4->setOpacity(0.0f);
    text5->setOpacity(0.0f);
    playAgain->setOpacity(0.0f);
    quit->setOpacity(0.0f);
    playAgainWindow->setOpacity(0.0f);
    quitHover->setOpacity(0.0f);
    playAgainHover->setOpacity(0.0f);
    phase = 0;

    GameManager::getInstance()->addKeyCallback(GLFW_KEY_9, true, [this]() {
        this->runOutro();
    });
}

void OutroCutscene::update(float msec) {

    if(run) {
        switch (phase) {
            case 0:
                if (transitionPlane->getOpacity() > 0.0f) {
                    transitionPlane->setOpacity(elapsed);
                    elapsed -= 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 1;
                }
                break;
            case 1:
                if (text1->getOpacity() < 1.0f) {
                    text1->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 2;
                }
                break;
            case 2:
                if (text2->getOpacity() < 1.0f) {
                    text2->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 3;
                }
                break;
            case 3:
                if (text3->getOpacity() < 1.0f) {
                    text3->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 4;
                }
                break;
            case 4:
                if (text4->getOpacity() < 1.0f) {
                    text4->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 5;
                }
                break;
            case 5:
                if (text5->getOpacity() < 1.0f) {
                    text5->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 6;
                }
                break;
            case 6:
                if (transitionPlane->getOpacity() < 1.0f) {
                    transitionPlane->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    phase = 7;
                    player->getComponent<Picking>()->showPreview();
                }
                break;
            case 7:
                paperTexture->setActive(false);
                text1->setActive(false);
                text2->setActive(false);
                text3->setActive(false);
                text4->setActive(false);
                text5->setActive(false);

                if (playAgainWindow->getOpacity() < 1.0f) {
                    playAgainWindow->setOpacity(elapsed);
                    elapsed += 0.5f * msec;
                    if(playAgainWindow->getOpacity() >= 1.0f) setMenuCallbacks();
                }
                break;
        }
    }
}

OutroCutscene::~OutroCutscene() {}

SerializableType OutroCutscene::getSerializableType() {
    return SOutroCutscene;
}

Json::Value OutroCutscene::serialize(Serializer* serializer) {
    Json::Value root = Component::serialize(serializer);
    root["scene"] = serializer->serialize(scene);
    root["player"] = serializer->serialize(player);
    root["sunController"] = serializer->serialize(sunController);
    return root;
}

void OutroCutscene::deserialize(Json::Value& root, Serializer* serializer) {
    Component::deserialize(root, serializer);
    scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
    player = dynamic_cast<GraphNode*>(serializer->deserialize(root["player"]).object);
    sunController = dynamic_cast<SunController*>(serializer->deserialize(root["sunController"]).object);
    initialize();
}