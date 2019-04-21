#ifndef MESHBOX_H
#define MESHBOX_H

#include "Global.h"
#include "MeshTexture.h"

class MeshBox : public MeshTexture {
public:
	MeshBox(glm::vec3 dimensions, std::string texturePath);
	MeshBox(glm::vec3 min, glm::vec3 max, std::string texturePath);
	void updateValues(glm::vec3 min, glm::vec3 max);
	void updateValues(glm::vec3 dimensions);
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
	glm::vec3 getDimensions() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshBox() {}
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
