//
// Created by MB on 5/26/2019.
//

#ifndef TEAROOM_MENUPREVIEW_H
#define TEAROOM_MENUPREVIEW_H


#include "Scene.h"

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

};


#endif //TEAROOM_MENUPREVIEW_H
