#ifndef MESHCOLORTORUS_H
#define MESHCOLORTORUS_H

#include "Headers.h"
#include <vector>
#include "MeshSimple.h"

class MeshColorTorus : public MeshSimple {
public:
	MeshColorTorus(Shader shader, float radiusIn, float radiusOut, int sideAmount, glm::vec4 color,
	               glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radiusIn, float radiusOut, int sideAmount);
	void drawGui(bool autoUpdate = true) override;
	glm::vec3 baseCenter;
protected:
	void createTorusSegment(std::vector<SimpleVertex>* vertices, float angle, float radStep);
	void createRectangle(std::vector<SimpleVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL);
	void bufferData(std::vector<SimpleVertex>* vertices);
	void setupMesh() override;
	float radiusIn;
	float radiusOut;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
