//
// Created by MB on 4/28/2019.
//

#ifndef TEAROOM_SUNCONTROLLER_H
#define TEAROOM_SUNCONTROLLER_H

#include "Scene/Components/Component.h"
#include "Scene/Scenes/Scene.h"

class SunController {
public:
    SunController(GraphNode* _gameObject, const std::string& name, Scene* scene);

protected:
    Scene* scene;
    UiColorPlane* clockBackground;

};
#endif //TEAROOM_SUNCONTROLLER_H
