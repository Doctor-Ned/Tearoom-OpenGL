//
// Created by MB on 5/26/2019.
//

#include <Ui/UiButton.h>
#include "MenuPreview.h"
#include <Ui/UiColorPlane.h>
#include "MenuScene.h"

MenuPreview::MenuPreview() {
    UiPlane*  menuBackground = new UiPlane("res/textures/menuSideBar.PNG", glm::vec2(-1.0f, -10.0f), glm::vec2(380.0f, 800.0f), TopLeft);
    UiButton* startGame = new UiButton(glm::vec2(98.0f, 210.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* options = new UiButton(glm::vec2(98.0f, 310.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* about = new UiButton(glm::vec2(98.0f, 410.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* quit = new UiButton(glm::vec2(98.0f, 500.0f), glm::vec2(170.0f,80.0f), Left);

    options->setOpacity(0.0f);
    startGame->setOpacity(0.0f);
    about->setOpacity(0.0f);
    quit->setOpacity(0.0f);

    UiColorPlane* startHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 210.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* optionsHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 310.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* aboutHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 405.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* quitHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 503.0f), glm::vec2(170.0f,80.0f), Left);

    startGame->addClickCallback([this]() {
        GameManager::getInstance()->setCurrentScene(new MenuScene());
    });
    startGame->addHoverCallback([this, startHover](){
        startHover->setOpacity(0.3f);
    });
    startGame->addLeaveCallback([this, startHover](){
        startHover->setOpacity(0.0f);
    });

    options->addHoverCallback([this, optionsHover](){
        optionsHover->setOpacity(0.3f);
    });
    options->addLeaveCallback([this, optionsHover](){
        optionsHover->setOpacity(0.0f);
    });

    about->addHoverCallback([this, aboutHover](){
        aboutHover->setOpacity(0.3f);
    });
    about->addLeaveCallback([this, aboutHover](){
        aboutHover->setOpacity(0.0f);
    });

    quit->addClickCallback([this]() {
        GameManager::getInstance()->quit();
    });
    quit->addHoverCallback([this, quitHover](){
        quitHover->setOpacity(0.3f);
    });
    quit->addLeaveCallback([this, quitHover](){
        quitHover->setOpacity(0.0f);
    });

    rootUiElement->addChild(menuBackground);
    rootUiElement->addChild(startGame);
    rootUiElement->addChild(options);
    rootUiElement->addChild(about);
    rootUiElement->addChild(quit);

    rootUiElement->addChild(startHover);
    rootUiElement->addChild(optionsHover);
    rootUiElement->addChild(aboutHover);
    rootUiElement->addChild(quitHover);

    startHover->setOpacity(0.0f);
    optionsHover->setOpacity(0.0f);
    aboutHover->setOpacity(0.0f);
    quitHover->setOpacity(0.0f);
}

MenuPreview::~MenuPreview() {}