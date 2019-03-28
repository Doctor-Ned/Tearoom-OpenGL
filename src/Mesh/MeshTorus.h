#ifndef MESHTORUS_H
#define MESHTORUS_H

#include "Headers.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshTorus : public MeshTexture {
public:
	MeshTorus(Shader shader, float radiusIn, float radiusOut, int sideAmount, char* texturePath,
	          glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radiusIn, float radiusOut, int sideAmount);
	void drawGui(bool autoUpdate = true) override;
	glm::vec3 baseCenter;
protected:
	void createTorusSegment(std::vector<TextureVertex>* vertices, float angle, float radStep);
	void createRectangle(std::vector<TextureVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL);
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh() override;
	float radiusIn;
	float radiusOut;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
