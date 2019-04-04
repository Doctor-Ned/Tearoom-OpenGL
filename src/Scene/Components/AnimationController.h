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
    glm::mat4 transformMatrix;
    glm::float32 velocity;
    AnimationType animType;
public:
    virtual ~AnimationController();

    AnimationController(AnimationType _type, Mesh *mesh, GraphNode* _gameObject);
    void update() override;
    void draw() override;
    void startAnimation();
    void calculatePath();
};


#endif //TEAROOM_ANIMATION_H
