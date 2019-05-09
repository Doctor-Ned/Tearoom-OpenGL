#pragma once

#include "LightComp.h"

struct DirLight;

class DirLightComp : public LightComp {
public:
	DirLightComp(DirLight *light, GraphNode *parent);
	DirLight *getLight() const;
	glm::vec4 getColor() override;
	float getStrength();
	void setColor(glm::vec4 color) override;
	void setModel(glm::mat4 model) override;
	void setStrength(float strength);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
private:
	void renderGui() override;
	bool getEnabled() override;
	void setEnabled(bool enabled) override;
	DirLight *light;
	DirLightComp(){}
	friend class Serializer;
};
