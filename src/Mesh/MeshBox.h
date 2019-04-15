#ifndef MESHBOX_H
#define MESHBOX_H

#include "Global.h"
#include "MeshTexture.h"

class MeshBox : public MeshTexture {
public:
	MeshBox(glm::vec3 dimensions, char* texturePath);
	MeshBox(glm::vec3 min, glm::vec3 max, char* texturePath);
	void updateValues(glm::vec3 min, glm::vec3 max);
	glm::vec3 getMin();
	glm::vec3 getMax();
protected:
	void draw(Shader *shader, glm::mat4 world) override;
	void setupMesh();
	glm::vec3 min, max;
	unsigned int vertexAmount;
};

#endif
