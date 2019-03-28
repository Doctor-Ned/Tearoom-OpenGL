#ifndef MESHCONE_H
#define MESHCONE_H

#include "Headers.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshCone : public MeshTexture {
public:
	MeshCone(Shader shader, float radius, float height, int sideAmount, char* texturePath,
	         glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radius, float height, int sideAmount);
	void drawGui(bool autoUpdate = true) override;
	glm::vec3 baseCenter;
protected:
	void createBottomTriangle(std::vector<TextureVertex>* vertices, float angle1, float angle2);
	void createTopTriangle(std::vector<TextureVertex>* vertices);
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh() override;
	float height;
	float radius;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
