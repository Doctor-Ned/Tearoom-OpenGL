//
// Created by MB on 5/26/2019.
//

#ifndef TEAROOM_MENUPREVIEW_H
#define TEAROOM_MENUPREVIEW_H


#include "Scene.h"
#include "Ui/UiColorPlane.h"

class MenuPreview : public Scene {
public:
    MenuPreview();
    ~MenuPreview();
    void update(double deltaTime) override;

protected:
    std::vector<UiPlane*> slidePhotos;
    double elapsed = 0.0;
    int currentPhoto = 1;
    int previousPhoto = 0;
    bool startTransition = false;
    bool runSlideShow = true;
    UiColorPlane* transitionPlane = new UiColorPlane(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 800.0f), TopLeft);
};

#endif //TEAROOM_MENUPREVIEW_H
