#ifndef MESHCOLORBOX_H
#define MESHCOLORBOX_H

#include "Global.h"
#include "MeshSimple.h"

class MeshColorBox : public MeshSimple {
public:
	MeshColorBox(glm::vec3 dimensions, glm::vec4 color);
	MeshColorBox(glm::vec3 min, glm::vec3 max, glm::vec4 color);
	void updateValues(glm::vec3 min, glm::vec3 max);
	void updateValues(glm::vec3 dimensions);
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
	glm::vec3 getDimensions() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshColorBox(){}
	void renderGui() override;
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
