//
// Created by MB on 5/29/2019.
//

#ifndef TEAROOM_INTROCUTSCENE_H
#define TEAROOM_INTROCUTSCENE_H

#include <Ui/UiColorPlane.h>
#include <Ui/UiPlane.h>
#include <Ui/UiCanvas.h>
#include <Ui/UiText.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"

class IntroCutscene: public Component {
public:
    IntroCutscene(Scene* scene);
    ~IntroCutscene() override;
    void update(float msec) override;
    void turnOff();
    void turnOn();
    void runIntro();
    void showNext(UiElement* uiElement);
protected:
    float elapsed = 1.0f;
    short phase = 0;
    UiCanvas* mainCanvas;
    bool run = false;
    bool planeSwitch = false;
    UiPlane* paperTexture = new UiPlane("res/textures/cutscenePaper.PNG", glm::vec2(350.0f, 50.0f), glm::vec2(700.0f, 700.0f), TopLeft);;
    UiColorPlane* transitionPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 800.0f), TopLeft);
    UiPlane* text1 = new UiPlane("res/textures/text1.PNG", glm::vec2(500.0f, 150.0f), glm::vec2(400.0f, 100.0f), TopLeft);
    UiPlane* text2 = new UiPlane("res/textures/text2.PNG", glm::vec2(500.0f, 250.0f), glm::vec2(400.0f, 100.0f), TopLeft);
    UiPlane* text3 = new UiPlane("res/textures/text3.PNG", glm::vec2(500.0f, 350.0f), glm::vec2(400.0f, 100.0f), TopLeft);
    UiPlane* text4 = new UiPlane("res/textures/text4.PNG", glm::vec2(500.0f, 450.0f), glm::vec2(400.0f, 100.0f), TopLeft);
    UiPlane* text5 = new UiPlane("res/textures/text5.PNG", glm::vec2(500.0f, 550.0f), glm::vec2(100.0f, 50.0f), TopLeft);
};


#endif //TEAROOM_INTROCUTSCENE_H
