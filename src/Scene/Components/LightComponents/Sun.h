#pragma once

#include "DirLightComp.h"

class LightManager;

class Sun : public Component {
public:
	Sun(DirLight *light1, DirLight *light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor, glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject);
	Sun(DirLight *light1, DirLight *light2, glm::vec4 dawnColor, glm::vec4 dayColor, glm::vec4 duskColor, glm::vec4 nightColor, float sunDistance, float initialTime, GraphNode *gameObject, DirLightComp *dirLightComp1, DirLightComp *dirLightComp2);
	void setTime(float time); //in hours
	void addTime(float time);
	void setVerticalRotation(float angle); //in degrees
	float getTime();
	void update(float msec) override;
	void renderGui() override;
	void setComponentActive(bool active) override;
	float getAmbientFactor();
	void setAmbientFactor(float ambientFactor);
	float getSpecularFactor();
	void setSpecularFactor(float specularFactor);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	float ambientFactor = 0.01f;
	float specularFactor = 0.3f;
	bool dirty = false;
	float rescaleTime(float time);
	glm::vec4 timeToColor(float time, bool light1);
	float time, rotationAngle;
	glm::vec4 dawnColor, dayColor, duskColor, nightColor;
	DirLight *light1, *light2;
	DirLightComp *light1Comp, *light2Comp;
	float sunDistance;
	Sun(){}
	friend class Serializer;
};
