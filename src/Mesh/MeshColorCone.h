#ifndef MESHCOLORCONE_H
#define MESHCOLORCONE_H

#include "Headers.h"
#include <vector>
#include "MeshSimple.h"

class MeshColorCone : public MeshSimple {
public:
	MeshColorCone(Shader shader, float radius, float height, int sideAmount, glm::vec4 color,
	              glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radius, float height, int sideAmount);
	void drawGui(bool autoUpdate = true) override;
	glm::vec3 baseCenter;
protected:
	void createBottomTriangle(std::vector<SimpleVertex>* vertices, float angle1, float angle2);
	void createTopTriangle(std::vector<SimpleVertex>* vertices);
	void bufferData(std::vector<SimpleVertex>* vertices);
	void setupMesh() override;
	float height;
	float radius;
	int sideAmount;
	unsigned int vertexAmount;
};

#endif
