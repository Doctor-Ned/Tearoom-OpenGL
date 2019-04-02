#pragma once

#include "LightNode.h"

class DirLightNode : public LightNode {
public:
	DirLightNode(DirLight *light, Mesh *mesh, GraphNode *parent);
	glm::vec4 getAmbient() override;
	glm::vec4 getDiffuse() override;
	glm::vec4 getSpecular() override;
	void setAmbient(glm::vec4 ambient) override;
	void setDiffuse(glm::vec4 diffuse) override;
	void setSpecular(glm::vec4 specular) override;
	DirLight *getLight() const;
protected:
	void setModel(glm::mat4 model) override;
private:
	DirLight *light;
};
