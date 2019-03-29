#ifndef MESHCOLORPLANE_H
#define MESHCOLORPLANE_H

#include "Headers.h"
#include "MeshSimple.h"

class MeshColorPlane : public MeshSimple {
public:
	MeshColorPlane(Shader *shader, float width, float length, glm::vec4 color,
	               glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void draw(Shader *shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(float width, float length);
	glm::vec3 baseCenter;
protected:
	void setupMesh();
	float width;
	float length;
	unsigned int vertexAmount;
};

#endif
