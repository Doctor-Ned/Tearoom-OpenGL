#pragma once

#include "LightNode.h"

class DirLightNode : public LightNode {
public:
	DirLightNode(DirLight *light, Mesh *mesh, GraphNode *parent);
	DirLight *getLight() const;
protected:
	glm::vec4 getLightAmbient() override;
	glm::vec4 getLightDiffuse() override;
	glm::vec4 getLightSpecular() override;
	void setLightAmbient(glm::vec4 ambient) override;
	void setLightDiffuse(glm::vec4 diffuse) override;
	void setLightSpecular(glm::vec4 specular) override;
	void setModel(glm::mat4 model) override;
private:
	DirLight *light;
};
