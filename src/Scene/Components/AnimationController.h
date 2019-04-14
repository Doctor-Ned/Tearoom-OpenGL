//
// Created by MB on 4/3/2019.
//

#ifndef TEAROOM_ANIMATION_H
#define TEAROOM_ANIMATION_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"

enum AnimationType {
    DoorOpeningX,
    DoorClosingX,
    DoorOpeningY,
    DoorClosingY,
    SafePullOutX,
    SafePullOutY
};

class AnimationController: public Component, public Renderable
{

protected:
    bool *F_keyState;
	bool animating = false;
	glm::mat4 transformMatrix;
    Mesh* mesh = nullptr;
    AnimationType type;
    glm::float32 elapsed = 0.0f;

public:
	bool isOpaque() override;
    virtual ~AnimationController();
    AnimationController(AnimationType _type, GraphNode* _gameObject, bool* f_keyPressed);
    void update(float msec) override;
	void updateDrawData() override;
	void drawSelf(Shader* shader) override;
	ShaderType getShaderType() override;
    void startAnimation();
};

#endif //TEAROOM_ANIMATION_H
