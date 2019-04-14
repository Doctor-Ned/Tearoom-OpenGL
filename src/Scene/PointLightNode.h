#pragma once

#include "QuadraticLightNode.h"

class PointLightNode : public QuadraticLightNode {
public:
	PointLightNode(PointLight *light, Mesh *mesh, GraphNode *parent);
	float getConstant() override;
	float getLinear() override;
	float getQuadratic() override;
	void setConstant(float constant) override;
	void setLinear(float linear) override;
	void setQuadratic(float quadratic) override;
	PointLight *getLight() const;
protected:
	glm::vec4 getLightAmbient() override;
	glm::vec4 getLightDiffuse() override;
	glm::vec4 getLightSpecular() override;
	void setLightAmbient(glm::vec4 ambient) override;
	void setLightDiffuse(glm::vec4 diffuse) override;
	void setLightSpecular(glm::vec4 specular) override;
	void setModel(glm::mat4 model) override;
private:
	PointLight *light;
};