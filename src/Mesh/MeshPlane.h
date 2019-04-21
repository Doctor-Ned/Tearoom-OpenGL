#ifndef MESHPLANE_H
#define MESHPLANE_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshPlane : public MeshTexture {
public:
	MeshPlane(float width, float length, std::string texturePath,
	          glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float width, float length);
	void updateValues(float width, float length, glm::vec3 baseCenter);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshPlane() {}
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	glm::vec3 baseCenter;
	float width;
	float length;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
