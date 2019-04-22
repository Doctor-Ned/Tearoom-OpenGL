#pragma once

#include "LightComp.h"

struct DirLight;

class DirLightComp : public LightComp {
public:
	DirLightComp(DirLight *light, GraphNode *parent);
	DirLight *getLight() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
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
private:
	DirLight *light;
	DirLightComp(){}
	friend class Serializer;
};
