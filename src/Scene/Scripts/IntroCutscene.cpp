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
#include "IntroCutscene.h"

IntroCutscene::IntroCutscene(Scene* scene) {
    UiElement *root = scene->getUiRoot();
    mainCanvas = new UiCanvas(glm::vec2(0.0f, 0.0f), root->getSize());
    root->addChild(mainCanvas);
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

}

void IntroCutscene::runIntro() {
    run = true;
}

void IntroCutscene::showNext(UiElement* uiElement) {
    while (elapsed <= 1.0f) {
        uiElement->setOpacity(elapsed);
        elapsed += 0.01f;
    }
    elapsed = 0.0f;
}

void IntroCutscene::update(float msec) {

    if(run) {
        switch (phase) {
            case 0:
            if (transitionPlane->getOpacity() > 0.0f) {
                transitionPlane->setOpacity(elapsed);
                std::cout<<elapsed<<std::endl;
                elapsed -= 0.01f;
            } else {
                elapsed = 0.0f;
                phase = 1;
            }
            break;
            case 1:
                if (text1->getOpacity() < 1.0f) {
                    text1->setOpacity(elapsed);
                    std::cout<<elapsed<<std::endl;

                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    phase = 2;
                }
                break;
            case 2:
                if (text2->getOpacity() < 1.0f) {
                    text2->setOpacity(elapsed);
                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    phase = 3;
                }
                break;
            case 3:
                if (text3->getOpacity() < 1.0f) {
                    text3->setOpacity(elapsed);
                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    phase = 4;
                }
                break;
            case 4:
                if (text4->getOpacity() < 1.0f) {
                    text4->setOpacity(elapsed);
                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    phase = 5;
                }
                break;
            case 5:
                if (text5->getOpacity() < 1.0f) {
                    text5->setOpacity(elapsed);
                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    phase = 5;
                }
                break;
            case 6:
                if (transitionPlane->getOpacity() < 1.0f) {
                    transitionPlane->setOpacity(elapsed);
                    elapsed += 0.01f;
                } else {
                    elapsed = 0.0f;
                    run = false;
                }
                break;
        }
    }
}

IntroCutscene::~IntroCutscene() {}

