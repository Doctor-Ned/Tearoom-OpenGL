#ifndef MESHSPHERE_H
#define MESHSPHERE_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>

class MeshSphere : public MeshTexture {
public:
	MeshSphere(float radius, int precision, std::string texturePath, glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float radius, int precision);
	void updateValues(float radius, int precision, glm::vec3 baseCenter);
	float getRadius() const;
	SerializableType getSerializableType() override;
	Json::Value serialize(Serializer* serializer) override;
	void deserialize(Json::Value& root, Serializer* serializer) override;
protected:
	MeshSphere() {}
	void draw(Shader *shader, glm::mat4 world) override;
	void createSphereSegment(std::vector<TextureVertex>* vertices, float angle, float radStep);
	void createRectangle(std::vector<TextureVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL) const;
	void createTriangle(std::vector<TextureVertex>* vertices, glm::vec3* up, glm::vec3* right, glm::vec3* left);
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh();
	glm::vec3 baseCenter;
	float radius;
	int precision;
	unsigned int vertexAmount;
	friend class Serializer;
};

#endif
