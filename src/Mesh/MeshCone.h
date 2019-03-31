#ifndef MESHCONE_H
#define MESHCONE_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshCone : public MeshTexture {
public:
	MeshCone(float radius, float height, int sideAmount, char* texturePath,
	         glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader *shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radius, float height, int sideAmount);
	glm::vec3 baseCenter;
protected:
	void createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2) const;
	void createTopTriangle(std::vector<TextureVertex>* vertices) const;
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh();
	float height;
	float radius;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
