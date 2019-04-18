#pragma once

#include "QuadraticLightNode.h"

class SpotLightNode : public QuadraticLightNode {
public:
	SpotLightNode(SpotLight *light, Mesh *mesh, GraphNode *parent);
	float getConstant() override;
	float getLinear() override;
	float getQuadratic() override;
	float getCutoff();
	float getOuterCutoff();
	void setConstant(float constant) override;
	void setLinear(float linear) override;
	void setQuadratic(float quadratic) override;
	void setCutoff(float cutoff);
	void setOuterCutoff(float outerCutoff);
	SpotLight *getLight() const;
protected:
	bool getEnabled() override;
	void setEnabled(bool enabled) override;
	glm::vec4 getAmbient() override;
	glm::vec4 getDiffuse() override;
	glm::vec4 getSpecular() override;
	void setAmbient(glm::vec4 ambient) override;
	void setDiffuse(glm::vec4 diffuse) override;
	void setSpecular(glm::vec4 specular) override;
	void setModel(glm::mat4 model) override;
	void renderGui() override;
private:
	SpotLight *light;
};