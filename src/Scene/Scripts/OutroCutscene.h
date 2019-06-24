//
// Created by MB on 6/11/2019.
//

#ifndef TEAROOM_OUTROCUTSCENE_H
#define TEAROOM_OUTROCUTSCENE_H

#include <Ui/UiColorPlane.h>
#include <Ui/UiPlane.h>
#include <Ui/UiCanvas.h>
#include <Ui/UiText.h>
#include <Ui/UiButton.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"

class SunController;

class OutroCutscene: public Component {
public:
    OutroCutscene(Scene* scene, SunController *sunController, GraphNode* player);
    ~OutroCutscene() override;
    SerializableType getSerializableType() override;
    Json::Value serialize(Serializer *serializer) override;
    void deserialize(Json::Value &root, Serializer* serializer) override;
    void update(float msec) override;
    void runOutro();
    void setMenuCallbacks();
protected:
    void initialize();
    SunController *sunController;
    friend class Serializer;
    OutroCutscene(){}
    GraphNode* player;
    Scene* scene;
    float elapsed = 1.0f;
    short phase = 0;
    UiCanvas* mainCanvas;
    bool run = false;
    UiPlane* paperTexture = new UiPlane("res/textures/cutscenePaper.PNG", glm::vec2(350.0f, 50.0f), glm::vec2(600.0f, 600.0f), TopLeft);;
    UiColorPlane* backgroundPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 800.0f), TopLeft);

    UiPlane* text1 =  new UiPlane("res/textures/Outro/outroLine1.PNG", glm::vec2(400.0f, 100.0f), glm::vec2(500.0f, 85.0f), TopLeft);
    UiPlane* text2 =  new UiPlane("res/textures/Outro/outroLine2.PNG", glm::vec2(400.0f, 180.0f), glm::vec2(520.0f, 120.0f), TopLeft);
    UiPlane* text3 =  new UiPlane("res/textures/Outro/outroLine3.PNG", glm::vec2(400.0f, 290.0f), glm::vec2(515.0f, 92.0f), TopLeft);
    UiPlane* text4 =  new UiPlane("res/textures/Outro/outroLine4.PNG", glm::vec2(405.0f, 380.0f), glm::vec2(500.0f, 120.0f), TopLeft);
    UiPlane* text5 =  new UiPlane("res/textures/Outro/outroLine5.PNG", glm::vec2(400.0f, 520.0f), glm::vec2(480.0f, 75.0f), TopLeft);
    UiPlane* playAgainWindow = new UiPlane("res/textures/Outro/playAgainWindow.PNG", glm::vec2(400.0f, 220.0f), glm::vec2(480.0f, 250.0f), TopLeft);

    UiButton* quit = new UiButton(glm::vec2(680.0f, 380.0f), glm::vec2(100.0f,60.0f), Left);
    UiButton* playAgain = new UiButton(glm::vec2(500.0f, 380.0f), glm::vec2(100.0f,60.0f), Left);

    UiColorPlane* quitHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(680.0f, 380.0f), glm::vec2(100.0f,60.0f), Left);
    UiColorPlane* playAgainHover = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.8f), glm::vec2(500.0f, 380.0f), glm::vec2(100.0f,60.0f), Left);
};
#endif //TEAROOM_OUTROCUTSCENE_H
