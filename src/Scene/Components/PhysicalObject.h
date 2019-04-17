//
// Created by MB on 4/13/2019.
//
#ifndef TEAROOM_OPENGL_PHYSICALOBJECT_H
#define TEAROOM_OPENGL_PHYSICALOBJECT_H
#include "Component.h"
#include "Mesh/Mesh.h"
#include "Render/Renderable.h"
#include <queue>

class PhysicalObject: public Component, public Renderable {
public:
	bool isActive() override;
    PhysicalObject(GraphNode* _gameObject);
    virtual ~PhysicalObject();
    void update(float msec) override;
    void updateDrawData() override;
    void drawSelf(Shader* shader) override;
    ShaderType getShaderType() override;
	bool isOpaque() override;
    bool castRayDown();
	void pushTranslation(glm::vec3 translation);
	glm::vec3 direction = glm::vec3(0);
	float distance = 0.0f;
protected:
	std::queue<glm::vec3> lastKnownPosition;
	std::queue<glm::vec3> translations;
    bool isFalling = false;
    Mesh* mesh = nullptr;
	bool gravity = false;
};


#endif //TEAROOM_OPENGL_PHYSICALOBJECT_H
