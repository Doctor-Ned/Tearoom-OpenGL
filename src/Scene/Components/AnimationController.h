//
// Created by MB on 4/3/2019.
//

#ifndef TEAROOM_ANIMATION_H
#define TEAROOM_ANIMATION_H
#include "Component.h"
#include "Mesh/Mesh.h"

enum AnimationType {
    SafePullOut,
    DoorOpening,
    DoorClosing
};

class AnimationController: public Component
{
protected:
    Mesh* mesh;
    AnimationType type;
    bool playAnimation = false;
    glm::float32 elapsed = 0.0f;
public:
    virtual ~AnimationController();
    AnimationController(AnimationType _type, GraphNode* _gameObject);
    void update(float msec) override;
    void draw() override;
    void startAnimation();
    void calculatePath();
};

#endif //TEAROOM_ANIMATION_H
