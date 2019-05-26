//
// Created by MB on 5/26/2019.
//

#include <Ui/UiButton.h>
#include "MenuPreview.h"
#include <Ui/UiColorPlane.h>
#include "MenuScene.h"
#include "MiszukScene.h"

MenuPreview::MenuPreview() {
    UiPlane*  menuBackground = new UiPlane("res/textures/menuSideBar.PNG", glm::vec2(-1.0f, -10.0f), glm::vec2(380.0f, 800.0f), TopLeft);
    UiPlane*  menuAbout = new UiPlane("res/textures/aboutMenu.PNG", glm::vec2(-1.0f, -10.0f), glm::vec2(380.0f, 800.0f), TopLeft);
    UiButton* startGame = new UiButton(glm::vec2(98.0f, 210.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* options = new UiButton(glm::vec2(98.0f, 310.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* about = new UiButton(glm::vec2(98.0f, 410.0f), glm::vec2(170.0f,80.0f), Left);
    UiButton* quit = new UiButton(glm::vec2(98.0f, 500.0f), glm::vec2(170.0f,80.0f), Left);

    UiButton* backToMenu = new UiButton(glm::vec2(101.0f, 675.0f), glm::vec2(170.0f,80.0f), Left);


    options->setOpacity(0.0f);
    startGame->setOpacity(0.0f);
    about->setOpacity(0.0f);
    quit->setOpacity(0.0f);
    backToMenu->setOpacity(0.0f);

    UiColorPlane* startHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 210.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* optionsHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 310.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* aboutHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(98.0f, 405.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* quitHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(100.0f, 503.0f), glm::vec2(170.0f,80.0f), Left);
    UiColorPlane* backHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(100.0f, 678.0f), glm::vec2(170.0f,80.0f), Left);

    backToMenu->addHoverCallback([this, backHover]() {
        backHover->setOpacity(0.3f);
    });
    backToMenu->addLeaveCallback([this, backHover]() {
        backHover->setOpacity(0.0f);
    });
    backToMenu->addClickCallback([this, menuAbout, menuBackground, startGame, startHover,
                                         options, optionsHover, quit, quitHover, about, aboutHover, backToMenu, backHover]() {
        menuBackground->setActive(true);
        menuAbout->setActive(false);
        startGame->setActive(true);
        startHover->setActive(true);
        options->setActive(true);
        optionsHover->setActive(true);
        quit->setActive(true);
        quitHover->setActive(true);
        about->setActive(true);
        aboutHover->setActive(true);

        backToMenu->setActive(false);
        backHover->setActive(false);
    });
    startGame->addClickCallback([this]() {
        GameManager::getInstance()->setCurrentScene(new MiszukScene());
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

    about->addClickCallback([this, menuAbout, menuBackground, startGame, startHover,
                             options, optionsHover, quit, quitHover, about, aboutHover, backToMenu, backHover]() {
       menuBackground->setActive(false);
       menuAbout->setActive(true);
       startGame->setActive(false);
       startHover->setActive(false);
       options->setActive(false);
       optionsHover->setActive(false);
       quit->setActive(false);
       quitHover->setActive(false);
       about->setActive(false);
       aboutHover->setActive(false);

       backToMenu->setActive(true);
       backHover->setActive(true);
    });
    about->addHoverCallback([this, aboutHover](){
        aboutHover->setOpacity(0.3f);
    });
    about->addLeaveCallback([this, aboutHover](){
        aboutHover->setOpacity(0.0f);
    });
    quit->addClickCallback([this]() {
        //GameManager::getInstance()->quit();
        GameManager::getInstance()->setCurrentScene(new MenuScene()); //temporary
    });
    quit->addHoverCallback([this, quitHover](){
        quitHover->setOpacity(0.3f);
    });
    quit->addLeaveCallback([this, quitHover](){
        quitHover->setOpacity(0.0f);
    });


    UiPlane*  slide1 = new UiPlane("res/textures/photo1.jpg", glm::vec2(200.0f, 0.0f), glm::vec2(1100.0f, 800.0f), TopLeft);
    UiPlane*  slide2 = new UiPlane("res/textures/photo2.jpg", glm::vec2(200.0f, 0.0f), glm::vec2(1100.0f, 800.0f), TopLeft);
    UiPlane*  slide3 = new UiPlane("res/textures/photo3.jpg", glm::vec2(200.0f, 0.0f), glm::vec2(1100.0f, 800.0f), TopLeft);

    slidePhotos.push_back(slide1);
    slidePhotos.push_back(slide2);
    slidePhotos.push_back(slide3);


    for(UiPlane* photo : slidePhotos) {
        rootUiElement->addChild(photo);
    }
    rootUiElement->addChild(menuBackground);
    rootUiElement->addChild(startGame);
    rootUiElement->addChild(options);
    rootUiElement->addChild(about);
    rootUiElement->addChild(quit);

    rootUiElement->addChild(menuAbout);
    rootUiElement->addChild(backToMenu);
    rootUiElement->addChild(backHover);

    rootUiElement->addChild(startHover);
    rootUiElement->addChild(optionsHover);
    rootUiElement->addChild(aboutHover);
    rootUiElement->addChild(quitHover);

    startHover->setOpacity(0.0f);
    optionsHover->setOpacity(0.0f);
    aboutHover->setOpacity(0.0f);
    quitHover->setOpacity(0.0f);
    backHover->setOpacity(0.0f);
    menuAbout->setActive(false);
    backToMenu->setActive(false);

}

void MenuPreview::update(double deltaTime) {

    if(elapsed > 1.0f) {
        if(currentPhoto == slidePhotos.size() - 1) {
            currentPhoto = 0;
            previousPhoto = slidePhotos.size() - 1;
        }
        else {
            if(previousPhoto == slidePhotos.size() - 1) previousPhoto = 0;
            else previousPhoto++;
            currentPhoto++;
        }
        elapsed = 0.0f;
    }
    slidePhotos[previousPhoto]->setOpacity(1.0f - elapsed);
    slidePhotos[currentPhoto]->setOpacity(elapsed);

    elapsed += 0.01f;
}

MenuPreview::~MenuPreview() {}