//
// Created by MB on 5/29/2019.
//

#ifndef TEAROOM_INTROCUTSCENE_H
#define TEAROOM_INTROCUTSCENE_H

#include <Ui/UiColorPlane.h>
#include <Ui/UiPlane.h>
#include <Ui/UiCanvas.h>
#include <Ui/UiText.h>
#include <Ui/UiButton.h>
#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"

class SunController;

class IntroCutscene: public Component {
public:
    IntroCutscene(Scene* scene, SunController *sunController, GraphNode* player);
    ~IntroCutscene() override;
    SerializableType getSerializableType() override;
    Json::Value serialize(Serializer *serializer) override;
    void deserialize(Json::Value &root, Serializer* serializer) override;
    void update(float msec) override;
    void runIntro();
protected:
	void initialize();
	SunController *sunController;
    friend class Serializer;
	IntroCutscene(){}
    GraphNode* player;
    Scene* scene;
    float elapsed = 1.0f;
    short phase = 0;
    UiCanvas* mainCanvas;
	bool started = false;
    bool run = false;
    UiPlane* paperTexture = new UiPlane("res/textures/cutscenePaper.PNG", glm::vec2(350.0f, 50.0f), glm::vec2(600.0f, 600.0f), TopLeft);;
    UiColorPlane* backgroundPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 800.0f), TopLeft);
    UiColorPlane* transitionPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 800.0f), TopLeft);
    //intro sprites
	UiPlane* text1 = new UiPlane("res/textures/text1.PNG", glm::vec2(402.0f, 150.0f), glm::vec2(500.0f, 80.0f), TopLeft);
    UiPlane* text2 = new UiPlane("res/textures/text2.PNG", glm::vec2(400.0f, 230.0f), glm::vec2(500.0f, 54.0f), TopLeft);
    UiPlane* text3 = new UiPlane("res/textures/text3.PNG", glm::vec2(398.0f, 288.0f), glm::vec2(500.0f, 88.0f), TopLeft);
    UiPlane* text4 = new UiPlane("res/textures/text4.PNG", glm::vec2(400.0f, 380.0f), glm::vec2(500.0f, 85.0f), TopLeft);
    UiPlane* text5 = new UiPlane("res/textures/text5.PNG", glm::vec2(400.0f, 452.0f), glm::vec2(130.0f, 90.0f), TopLeft);

	UiPlane* skipSprite = new UiPlane("res/textures/skipSprite.PNG", glm::vec2(990.0f, 600.0f), glm::vec2(260.0f, 65.0f), TopLeft);
};


#endif //TEAROOM_INTROCUTSCENE_H
