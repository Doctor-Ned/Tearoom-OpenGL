//
// Created by MB on 4/3/2019.
//

#ifndef TEAROOM_ANIMATION_H
#define TEAROOM_ANIMATION_H
#include "Component.h"
#include "Mesh/Mesh.h"


class AnimationController: public Component {
public:
    AnimationController(Mesh *mesh);
    void update() override;
    void draw() override;
    void startAnimation();
    void calculatePath();
protected:
    Mesh* mesh;
    glm::mat4 newPosition;
    glm::float32 velocity;
};


#endif //TEAROOM_ANIMATION_H
