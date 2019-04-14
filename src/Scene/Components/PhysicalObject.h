//
// Created by MB on 4/13/2019.
//
#ifndef TEAROOM_OPENGL_PHYSICALOBJECT_H
#define TEAROOM_OPENGL_PHYSICALOBJECT_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"


class PhysicalObject: public Component, public Renderable {
public:
    PhysicalObject(GraphNode* _gameObject);
    virtual ~PhysicalObject();
    void update(float msec) override;
    void updateDrawData() override;
    void drawSelf(Shader* shader) override;
    ShaderType getShaderType() override;

    bool castRayDown();

protected:
    bool isFalling = false;
    Mesh* mesh;
};


#endif //TEAROOM_OPENGL_PHYSICALOBJECT_H
