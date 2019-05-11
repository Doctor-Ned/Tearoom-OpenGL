#pragma once

#include "QuadraticLightComp.h"

struct PointLight;

class PointLightComp : public QuadraticLightComp {
public:
	PointLightComp(PointLight *light, GraphNode *gameObject);
	float getConstant() override;
	float getLinear() override;
	float getQuadratic() override;
	PointLight *getLight() const;
	glm::vec4 getColor() override;
	float getNearPlane();
	float getFarPlane();
	void setConstant(float constant) override;
	void setLinear(float linear) override;
	void setQuadratic(float quadratic) override;
	void setColor(glm::vec4 color) override;
	void setModel(glm::mat4 model) override;
	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
private:
	void renderGui() override;
	bool getEnabled() override;
	void setEnabled(bool enabled) override;
	PointLight *light;
	PointLightComp(){}
	friend class Serializer;
};
