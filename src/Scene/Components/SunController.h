//
// Created by MB on 4/28/2019.
//

#ifndef TEAROOM_SUNCONTROLLER_H
#define TEAROOM_SUNCONTROLLER_H

#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"
#include "Ui/UiCanvas.h"
#include "Ui/UiColorPlane.h"

class Sun;
class SunController: public Component{
public:
    SunController(GraphNode* _gameObject, Scene* scene, const std::string& name = "SunController");
    ~SunController() override;
	void moveSun(float time);
    void update(float msec) override;
    SerializableType getSerializableType() override;
    Json::Value serialize(Serializer *serializer) override;
    void deserialize(Json::Value &root, Serializer* serializer) override;
private:
	void initialize();
	SunController();
    Scene* scene;
    UiColorPlane* clockBackground;
    //Clock UI
    bool clockUI = false;
	Sun* sun = nullptr;
    UiPlane* clockHand;
    UiPlane* clockFace;
    UiPlane* clockBack;
    friend class Serializer;

};
#endif //TEAROOM_SUNCONTROLLER_H
