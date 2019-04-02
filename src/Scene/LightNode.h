#pragma once

#include "GraphNode.h"
#include "GuiConfigurable.h"

class LightNode : public GraphNode {
public:
	LightNode(Mesh *mesh, GraphNode *parent) : GraphNode(mesh, parent) {}
	virtual glm::vec4 getAmbient() = 0;
	virtual glm::vec4 getDiffuse() = 0;
	virtual glm::vec4 getSpecular() = 0;
	virtual void setAmbient(glm::vec4 ambient) = 0;
	virtual void setDiffuse(glm::vec4 diffuse) = 0;
	virtual void setSpecular(glm::vec4 specular) = 0;
protected:
	void updateWorld() override;
	virtual void setModel(glm::mat4 model) = 0;
	void renderGui() override;
};
