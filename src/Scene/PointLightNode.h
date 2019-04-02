#pragma once

#include "QuadraticLightNode.h"

class PointLightNode : public QuadraticLightNode {
public:
	PointLightNode(PointLight *light, Mesh *mesh, GraphNode *parent);
	glm::vec4 getAmbient() override;
	glm::vec4 getDiffuse() override;
	glm::vec4 getSpecular() override;
	float getConstant() override;
	float getLinear() override;
	float getQuadratic() override;
	void setAmbient(glm::vec4 ambient) override;
	void setDiffuse(glm::vec4 diffuse) override;
	void setSpecular(glm::vec4 specular) override;
	void setConstant(float constant) override;
	void setLinear(float linear) override;
	void setQuadratic(float quadratic) override;
	PointLight *getLight() const;
protected:
	void setModel(glm::mat4 model) override;
private:
	PointLight *light;
};