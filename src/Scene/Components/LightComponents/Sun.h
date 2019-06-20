#pragma once

#include "DirLightComp.h"

class LightManager;

class Sun : public Component {
public:
	Sun(DirLight *light1, DirLight *light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor, glm::vec4 nightColor, float sunDistance, float initialTime, float rotationAngle, GraphNode *gameObject);
	Sun(DirLight *light1, DirLight *light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor, glm::vec4 nightColor, float sunDistance, float initialTime, float rotationAngle, GraphNode *gameObject, DirLightComp *dirLightComp1, DirLightComp *dirLightComp2);
	void setTime(float time); //in hours
	void addTime(float time);
	void setRotationAngle(float angle); //in radians
	float getTime();
	void update(float msec) override;
	void updateWorld() override;
	void renderGui() override;
	void setComponentActive(bool active) override;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	void recalculateMatrix();
	bool dirty = false;
	float rescaleTime(float time);
	glm::vec4 timeToColor(float time, bool light1);
	float time, rotationAngle;
	glm::vec4 dawnColor, dayColor, duskColor, nightColor;
	DirLight *light1, *light2;
	DirLightComp *light1Comp, *light2Comp;
	GraphNode *light1Node = nullptr, *light2Node = nullptr;
	float sunDistance;
	Sun(){}
	friend class Serializer;
};
