//
// Created by MB on 5/29/2019.
//
/*
 Skoro to czytasz, znaczy że już mnie nie ma. a mój dorobek na Ciebie czeka.

Nigdy nie mieliśmy czasu by razem napić się herbaty.

Zostawiam ci wszystko co posiadam w moim domu.

Mój testament czeka na ciebie, w schowku który oznaczyłem na zdjęciu.

Twój wujek,
Yoshiro
 */
#include <Scene/SoundSystem.h>
#include "IntroCutscene.h"
#include "CollectableObject.h"
#include "Picking.h"
#include "Scene/Components/SunController.h"
#include "Serialization/Serializer.h"

IntroCutscene::IntroCutscene(Scene* scene, SunController *sunController, GraphNode* player) : Component(player, "Intro Cutscene"), sunController(sunController) {
    this->scene = scene;
    this->player = player;
	initialize();
}

void IntroCutscene::runIntro() {
	gameManager->setCursorLocked(false);
    player->getComponent<Picking>()->hidePreview();

    if (sunController != nullptr) {
        sunController->setComponentActive(false);
        sunController->setClockVisibility(false);
    }
    run = true;
}

void IntroCutscene::switchToOutro() {
    text1 =  new UiPlane("res/textures/Outro/outroLine1.PNG", glm::vec2(400.0f, 150.0f), glm::vec2(500.0f, 90.0f), TopLeft);
    text2 =  new UiPlane("res/textures/Outro/outroLine2.PNG", glm::vec2(400.0f, 150.0f), glm::vec2(500.0f, 90.0f), TopLeft);
    text3 =  new UiPlane("res/textures/Outro/outroLine3.PNG", glm::vec2(400.0f, 150.0f), glm::vec2(500.0f, 90.0f), TopLeft);
    text4 =  new UiPlane("res/textures/Outro/outroLine4.PNG", glm::vec2(400.0f, 150.0f), glm::vec2(500.0f, 90.0f), TopLeft);
    text5 =  new UiPlane("res/textures/Outro/outroLine5.PNG", glm::vec2(400.0f, 150.0f), glm::vec2(500.0f, 90.0f), TopLeft);
}

void IntroCutscene::initialize() {
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
	transitionPlane->setOpacity(1.0f);
	text1->setOpacity(0.0f);
	text2->setOpacity(0.0f);
	text3->setOpacity(0.0f);
	text4->setOpacity(0.0f);
	text5->setOpacity(0.0f);
    phase = 0;
	GameManager::getInstance()->addKeyCallback(GLFW_KEY_8, true, [this]() {
		this->runIntro();
	});
}

void IntroCutscene::update(float msec) {

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
                    SoundSystem::getSound("slidingCutsceneDoor")->setDefaultVolume(0.60f);
                    SoundSystem::getEngine()->play2D(SoundSystem::getSound("slidingCutsceneDoor"));
                }
                break;
            case 6:
                if (transitionPlane->getOpacity() < 1.0f) {
                    transitionPlane->setOpacity(elapsed);
                    elapsed += 0.3f * msec;
                } else {
                    elapsed = 1.0f;
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

                if (backgroundPlane->getOpacity() > 0.0f) {
                    transitionPlane->setOpacity(elapsed);
                    backgroundPlane->setOpacity(elapsed);
                    elapsed -= 0.3f * msec;
                } else {
                    elapsed = 0.0f;
                    backgroundPlane->setActive(false);
                    transitionPlane->setActive(false);
                    run = false;
					gameManager->setCursorLocked(true);
                }
                break;
        }
    }
}

IntroCutscene::~IntroCutscene() {}

SerializableType IntroCutscene::getSerializableType() {
    return SIntroCutscene;
}

Json::Value IntroCutscene::serialize(Serializer* serializer) {
    Json::Value root = Component::serialize(serializer);
    root["scene"] = serializer->serialize(scene);
    root["player"] = serializer->serialize(player);
	root["sunController"] = serializer->serialize(sunController);
    return root;
}

void IntroCutscene::deserialize(Json::Value& root, Serializer* serializer) {
    Component::deserialize(root, serializer);
    scene = dynamic_cast<Scene*>(serializer->deserialize(root["scene"]).object);
    player = dynamic_cast<GraphNode*>(serializer->deserialize(root["player"]).object);
	sunController = dynamic_cast<SunController*>(serializer->deserialize(root["sunController"]).object);
	initialize();
}