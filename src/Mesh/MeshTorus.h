#ifndef MESHTORUS_H
#define MESHTORUS_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshTorus : public MeshTexture {
public:
	MeshTorus(float radiusIn, float radiusOut, int sideAmount, char* texturePath,
	          glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float radiusIn, float radiusOut, int sideAmount);
	glm::vec3 baseCenter;
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void createTorusSegment(std::vector<TextureVertex>* vertices, float angle, float radStep);
	void createRectangle(std::vector<TextureVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL);
	void bufferData(std::vector<TextureVertex>* vertices);
	void setupMesh();
	float radiusIn;
	float radiusOut;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
