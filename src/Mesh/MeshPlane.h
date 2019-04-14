#ifndef MESHPLANE_H
#define MESHPLANE_H

#include "Global.h"
#include "MeshTexture.h"
#include <string>
#include <vector>

class MeshPlane : public MeshTexture {
public:
	MeshPlane(float width, float length, char* texturePath,
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
