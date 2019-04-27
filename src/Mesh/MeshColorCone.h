#ifndef MESHCOLORCONE_H
#define MESHCOLORCONE_H

#include "Global.h"
#include <vector>
#include "MeshSimple.h"

class MeshColorCone : public MeshSimple {
public:
	MeshColorCone(float radius, float height, int sideAmount, glm::vec4 color,
	              glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float radius, float height, int sideAmount);
	void updateValues(float radius, float height, int sideAmount, glm::vec3 baseCenter);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshColorCone() {}
	void renderGui() override;
	void draw(Shader *shader, glm::mat4 world) override;
	void createBottomTriangle(std::vector<SimpleVertex>* vertices, float angle1, float angle2) const;
	void createTopTriangle(std::vector<SimpleVertex>* vertices) const;
	void bufferData(std::vector<SimpleVertex>* vertices);
	void setupMesh();
	glm::vec3 baseCenter;
	float height;
	float radius;
	int sideAmount;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
