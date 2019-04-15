#ifndef MESHCOLORPLANE_H
#define MESHCOLORPLANE_H

#include "Global.h"
#include "MeshSimple.h"

class MeshColorPlane : public MeshSimple {
public:
	MeshColorPlane(float width, float length, glm::vec4 color,
	               glm::vec3 baseCenter = glm::vec3(0.0f, 0.0f, 0.0f));
	void updateValues(float width, float length);
	glm::vec3 baseCenter;
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	float width;
	float length;
	unsigned int vertexAmount;
};

#endif
