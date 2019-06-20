#pragma once

#include "Mesh.h"

class MeshProperty abstract : public Mesh {
public:
	float getRoughness() const;
	float getMetallic() const;
	glm::vec3 getEmissive() const;
	virtual void setRoughness(float roughness);
	virtual void setMetallic(float metallic);
	virtual void setEmissive(glm::vec3 emissive);
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
	SerializableType getSerializableType() override;
protected:
	void renderGui() override;
	void draw(Shader *shader, glm::mat4 world) override;
	float roughness = 1.0f, metallic = 0.0f, speed = 1.0f, moveScale = 1.0f;
	bool moving = false;
	glm::vec3 emissive = glm::vec3(0.0f, 0.0f, 0.0f);
	MeshProperty() {}
	MeshProperty(ShaderType shaderType);
	friend class Serializer;
};