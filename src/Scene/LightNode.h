#pragma once

#include "GraphNode.h"
#include "GuiConfigurable.h"

class LightNode : public GraphNode {
public:
	LightNode(Mesh *mesh, GraphNode *parent) : GraphNode(mesh, parent) {}
	void setActive(bool active) override;
	glm::vec4 getAmbient();
	glm::vec4 getDiffuse();
	glm::vec4 getSpecular();
	void setAmbient(glm::vec4 ambient);
	void setDiffuse(glm::vec4 diffuse);
	void setSpecular(glm::vec4 specular);
protected:
	virtual glm::vec4 getLightAmbient() = 0;
	virtual glm::vec4 getLightDiffuse() = 0;
	virtual glm::vec4 getLightSpecular() = 0;
	virtual void setLightAmbient(glm::vec4 ambient) = 0;
	virtual void setLightDiffuse(glm::vec4 diffuse) = 0;
	virtual void setLightSpecular(glm::vec4 specular) = 0;
	void setAmbient(glm::vec4 ambient, bool force);
	void setDiffuse(glm::vec4 diffuse, bool force);
	void setSpecular(glm::vec4 specular, bool force);
	glm::vec4 getAmbient(bool force);
	glm::vec4 getDiffuse(bool force);
	glm::vec4 getSpecular(bool force);
	glm::vec4 lastAmbient, lastDiffuse, lastSpecular;
	void updateWorld() override;
	virtual void setModel(glm::mat4 model) = 0;
	void renderGui() override;
};
