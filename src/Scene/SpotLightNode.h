#ifndef SPOTLIGHTNODE_H
#define SPOTLIGHTNODE_H

#include "GraphNode.h"

class SpotLightNode : public GraphNode {
public:
	SpotLightNode(SpotLight* light, Mesh* mesh = nullptr, GraphNode* parent = nullptr);
	void update(double timeDiff) override;
	void drawGui(bool autoUpdate = true);
	SpotLight* getLight();
	float rotationZ;
	float rotationX;
protected:
	SpotLight* light;
	bool enabled = true;
	glm::vec4 lastAmbient;
	glm::vec4 lastDiffuse;
	glm::vec4 lastSpecular;
	float appliedZ;
	float appliedX;
	glm::vec4 pos;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	bool lastEnabled;
};

#endif
