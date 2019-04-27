#ifndef MESHCOLORTORUS_H
#define MESHCOLORTORUS_H

#include "Global.h"
#include <vector>
#include "MeshSimple.h"

class MeshColorTorus : public MeshSimple {
public:
	MeshColorTorus(float radiusIn, float radiusOut, int sideAmount, glm::vec4 color,
	               glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float radiusIn, float radiusOut, int sideAmount);
	void updateValues(float radiusIn, float radiusOut, int sideAmount, glm::vec3 baseCenter);
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshColorTorus() {}
	void renderGui() override;
	void draw(Shader *shader, glm::mat4 world) override;
	void createTorusSegment(std::vector<SimpleVertex>* vertices, float angle, float radStep) const;
	void createRectangle(std::vector<SimpleVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL) const;
	void bufferData(std::vector<SimpleVertex>* vertices);
	void setupMesh();
	glm::vec3 baseCenter;
	float radiusIn;
	float radiusOut;
	int sideAmount;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
