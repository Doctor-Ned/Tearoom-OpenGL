#pragma once

#include "QuadraticLightComp.h"

struct SpotLight;

class SpotLightComp : public QuadraticLightComp {
public:
	SpotLightComp(SpotLight *light, GraphNode *gameObject);
	float getConstant() override;
	float getLinear() override;
	float getQuadratic() override;
	float getNearPlane() override;
	float getFarPlane() override;
	float getCutoff();
	float getOuterCutoff();
	void setConstant(float constant) override;
	void setLinear(float linear) override;
	void setQuadratic(float quadratic) override;
	void setNearPlane(float nearPlane) override;
	void setFarPlane(float farPlane) override;
	void setCutoff(float cutoff);
	void setOuterCutoff(float outerCutoff);
	glm::vec4 getColor() override;
	void setColor(glm::vec4 color) override;
	void setModel(glm::mat4 model) override;
	SpotLight *getLight() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	bool getEnabled() override;
	void setEnabled(bool enabled) override;
	void renderGui() override;
private:
	SpotLight *light;
	SpotLightComp(){}
	friend class Serializer;
};
