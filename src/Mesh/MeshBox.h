#ifndef MESHBOX_H
#define MESHBOX_H

#include "Headers.h"
#include "MeshTexture.h"

class MeshBox : public MeshTexture {
public:
	MeshBox(Shader shader, glm::vec3 min, glm::vec3 max, char* texturePath);
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(glm::vec3 min, glm::vec3 max);
	glm::vec3 getMin();
	glm::vec3 getMax();
protected:
	void setupMesh() override;
	glm::vec3 min, max;
	unsigned int vertexAmount;
};

#endif
