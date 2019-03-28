#ifndef MESHREFBOX_H
#define MESHREFBOX_H

#include "Headers.h"
#include "MeshRef.h"

class MeshRefBox : public MeshRef {
public:
	MeshRefBox(Shader shader, glm::vec3 min, glm::vec3 max);
	void draw(Shader shader, glm::mat4 world, float scale = 1.0f) override;
	void updateValues(glm::vec3 min, glm::vec3 max);
	glm::vec3 getMin();
	glm::vec3 getMax();
protected:
	glm::vec3 getUnmodeledCenter();
	void setupMesh() override;
	glm::vec3 min, max;
	unsigned int vertexAmount;
};

#endif
