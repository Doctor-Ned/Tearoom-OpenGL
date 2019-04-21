#ifndef MESHCYLINDER_H
#define MESHCYLINDER_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshCylinder : public MeshTexture {
public:
	MeshCylinder(float radius, float height, int sideAmount, char* texturePath,
	             glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float radius, float height, int sideAmount);
	void updateValues(float radius, float height, int sideAmount, glm::vec3 baseCenter);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshCylinder() {}
	void draw(Shader *shader, glm::mat4 world) override;
	void createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2) const;
	void createTopTriangle(std::vector<TextureVertex>* vertices) const;
	void createSideTriangles(std::vector<TextureVertex>* vertices) const;
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh();
	glm::vec3 baseCenter;
	float height;
	float radius;
	int sideAmount;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
