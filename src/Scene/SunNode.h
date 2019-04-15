#pragma once

#include "DirLightNode.h"

class LightManager;

class SunNode : public GraphNode {
public:
	SunNode(DirLight *light1, DirLight *light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor, glm::vec4 nightColor, float sunDistance, float initialTime = 0.0f, GraphNode *parent = nullptr);
	void setTime(float time); //in hours
	void addTime(float time);
	void setVerticalRotation(float angle); //in degrees
protected:
	float rescaleTime(float time);
	glm::vec4 timeToColor(float time, bool light1);
	void updateWorld() override;
	float time, rotationAngle;
	glm::vec4 dawnColor, dayColor, duskColor, nightColor;
	DirLight *light1, *light2;
	DirLightNode *light1Node, *light2Node;
	float sunDistance;
};
