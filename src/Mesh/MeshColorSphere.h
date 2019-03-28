#ifndef MESHCOLORSPHERE_H
#define MESHCOLORSPHERE_H

#include "Headers.h"
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include "MeshSimple.h"

class MeshColorSphere : public MeshSimple {
public:
	MeshColorSphere(Shader shader, float radius, int precision, glm::vec4 color,
	                glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float radius, int precision);
	void drawGui(bool autoUpdate = true) override;
	float getRadius();
	glm::vec3 baseCenter;
protected:
	void createSphereSegment(std::vector<SimpleVertex>* vertices, float angle, float radStep);
	void createRectangle(std::vector<SimpleVertex>* vertices, glm::vec3* tL, glm::vec3* tR, glm::vec3* dR,
	                     glm::vec3* dL);
	void createTriangle(std::vector<SimpleVertex>* vertices, glm::vec3* up, glm::vec3* right, glm::vec3* left);
	void bufferData(std::vector<SimpleVertex>* vertices);
	void setupMesh() override;
	float radius;
	int precision;
	unsigned int vertexAmount;
};

#endif
